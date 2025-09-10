#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "./device/include/mailbox.h"
#include "./device/include/gps.h"
#include "./device/include/list.h"
#include "./device/include/ds18b20.h"
#include "./device/include/tds.h"
#include "./device/include/ts300b.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "./net/include/server.h"
#include "./device/include/gps.h"
#include "./device/include/uart.h"
#include <sqlite3.h>


#define TABLE_NAME "data"
struct list_head *pthreadhead = NULL;
float scale = 0;//分辨率
int fd_ts = 0;//TS_300B
int fd_tds = 0;//TDS
int fd_gps = 0;//gps
int fd_ds = 0;//gps
int sockfd = 0;
int uart_fd = 0;


static const char *filepath[] = {
    "/dev/ds18b20", 
    "/dev/ttymxc2",  //串口三
    "/sys/bus/iio/devices/iio:device0/in_voltage_scale", //分辨率
    "/sys/bus/iio/devices/iio:device0/in_voltage1_raw", //tds
    "/sys/bus/iio/devices/iio:device0/in_voltage4_raw",//ts300b
    "./Eirdata.db",
};
uart_cfg_t uart_conf = {
    .baudrate = 9600,
    .dbit = 8,
    .parity = 'N', 
    .sbit = 1,
};

void * collect_thread(void* arg)
{
    data_t data;
    Data Data;
    char res;
    int ret = 0,test = 3;
    unsigned char buf[10];
    int i = 0;
    tds_get_scale(filepath[2],&scale);
    printf("scale = %f\r\n",scale);
    
    fd_ds = open(filepath[0],O_RDONLY);
    if(fd_ds < 0 )
    {
        perror("can't open file :\r\n ");
    }
    

    while(1)
    {
       
        ds18b20_get_temp(fd_ds,&data.temperature); 
        tds_get_cond(filepath[3],&data.conductivity,&data.temperature,&scale);
        ts300b_get_tur(filepath[4],&data.turbidity,&data.temperature,&scale);
        
        printf("next\r\n");
        gps_get_data(filepath[1],&Data);
        data.latitude = (float)atof(Data.latitude);
        data.longitude = (float)atof(Data.longitude);
        //下侧添加ph，和gps位置
        printf("temp = %.4lf,conductivity = %.4lf,turbidity = %.4lf,latitude = %lf,longitude = %lf\r\n",\
                            data.temperature,data.conductivity,data.turbidity,data.latitude,data.longitude);
        mailbox_msgsend(pthreadhead,"显示线程",data);
        mailbox_msgsend(pthreadhead,"网络线程",data);
        mailbox_msgsend(pthreadhead,"存储线程",data);

        printf("采集线程采集数据完成\r\n");
        sleep(2);
    }

    
    close(fd_ds);
    return NULL;
}


void * show_thread(void *arg)
{
    data_t data;

    while(1)
    {
        mailbox_msgrecv (pthreadhead,&data);
          printf("展示线程正在关闭\r\n");
        sleep(3);
    }

    return NULL;

}
void *  net_thread(void* arg)
{
     struct sockaddr_in address;
    int addrlen = sizeof(address);
    struct epoll_event events[MAX_EVENTS];
    data_t data;
    int i = 0;
   
    
    // 初始化服务器
    if (init_server() != 0) {
        printf("failed to init server\r\n");
        return NULL;
    }
    printf("访问地址: http://%s:%d\n", SERVER_IP, PORT);
    // 运行服务器
    while (running) {
        mailbox_msgrecv(pthreadhead,&data);
        // 等待事件发生
        printf("网络线程正在运行:\r\n");
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        if (num_events == -1) {
            if (running && errno != EINTR) {
                perror("epoll_wait failed\r\n");
            }
            continue;
        }
        
        // 处理每个事件
        for (i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_fd) {
                // 新的连接请求
                int new_socket;
                while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) != -1) {
                    printf("新客户端连接: %s:%d\r\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    
                    if (add_client(new_socket) == -1) {
                        close(new_socket);
                        printf("无法添加客户端，可能已达到最大连接数\r\n");
                    }
                }
                
                // 非阻塞模式下，EAGAIN是正常的
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    perror("accept failed\r\n");
                }
            } else {
                // 客户端事件
                int client_index = find_client_index(events[i].data.fd);
                if (client_index == -1) {
                    continue; // 客户端不存在
                }
                
                if (events[i].events & EPOLLRDHUP || events[i].events & EPOLLHUP || events[i].events & EPOLLERR) {
                    // 客户端断开连接或出错
                    printf("客户端断开连接或出错\r\n");
                    remove_client(client_index);
                } else if (events[i].events & EPOLLIN) {
                    // 客户端数据到达
                    handle_client_data(client_index,&data);
                }
                
                // 如果响应已发送，关闭连接
                if (client_index != -1 && client_index < client_count && 
                    clients[client_index].fd == events[i].data.fd && 
                    clients[client_index].response_sent) {
                    remove_client(client_index);
                }
            }
        }
        printf("网络线程关闭\r\n");
        sleep(2);
    }
    return NULL;
}


void * save_thread(void* arg)
{
    data_t data;
    sqlite3 *ppDb;
    int ret;
    char sql[200] = {0};
    char *errmsg = NULL;
    
    ret = sqlite3_open(filepath[5],&ppDb);
    if(ret != SQLITE_OK)
    {
        printf("sqlite3_open failed!\r\n");

    }
    
    else{

        sprintf(sql,"CREATE TABLE '%s'(temperature REAL ,conductivity REAL,turbidity REAL,\
                        ph REAL, longitude REAL,latitude REAL);",TABLE_NAME);
  
        ret = sqlite3_exec(ppDb,sql,NULL,NULL,&errmsg);

        if(ret != SQLITE_OK)
        {
            printf("sqlite3_create failed:%s\r\n",sqlite3_errmsg(ppDb));

        }

    }
       
        
 
 
    while(1)
    {
        mailbox_msgrecv (pthreadhead,&data);
        

            memset(sql,0,sizeof(sql));
            sprintf(sql,"INSERT INTO %s VALUES(%f,%f,%f,%f,%f,%f);",\
                            TABLE_NAME,data.temperature,data.conductivity,data.turbidity,\
                        data.ph,data.longitude,data.latitude);
            ret = sqlite3_exec(ppDb,sql,NULL,NULL,&errmsg);
            if(ret != SQLITE_OK)
            {
                printf("insert fail %s\r\n",sqlite3_errmsg(ppDb));
            }



           
            
        printf("保存线程正在关闭\r\n");
        sleep(3);
    }
    sqlite3_close(ppDb);
    return NULL;

}


int main(int argc,char *argv[])
{
    void *(*pthreadfun[4])(void *) = {collect_thread,show_thread,net_thread,save_thread};
    char name[4][32] = {"采集线程","显示线程","网络线程","存储线程"};
    int client_count = 0;

    pthread_t tid[4];
    uart_init(filepath[1]);
    uart_cfg(&uart_conf);
    int i = 0;

    pthreadhead = mailbox_create();

    if (NULL == pthreadhead)
    {
        return -1;
    }

    for (i = 0; i < 4; i++)
    {
        mailbox_addtask(pthreadhead,tid[i],name[i],pthreadfun[i]);
    }

    while(1){

        sleep(5);
    }


 
        // 清理资源
    close(server_fd);
    close(epoll_fd);
    close(uart_fd);
    // 关闭所有客户端连接
    for (i = 0; i < client_count; i++) {
        close(clients[i].fd);
    }
    for (i = 0; i < 4; i++)
    {
        pthread_join(tid[i],NULL);
    }
    mailbox_destroy(pthreadhead);

    pthreadhead = NULL;

    printf("服务器已关闭\n");
    return 0;
}
