#ifndef __SERVER_H
#define __SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#include "server.h"
#include "../../device/include/mailbox.h"

#define PORT 8080
#define SERVER_IP "192.168.59.125"  // 监听所有接口
#define BUFFER_SIZE 4096
#define MAX_EVENTS 100
#define MAX_CLIENTS 50
#define DATA_REFRESH_RATE 1  // 数据刷新间隔(秒)
#define TIMEOUT 500         // epoll等待超时时间(毫秒)


// 传感器数据结构
typedef struct {
    float temperature;   // 温度 (℃)
    float conductivity;  // 电导率 (μS/cm)
    float turbidity;     // 浑浊度 (NTU)
    float ph;            // pH值
    float longitude;     // 经度
    float latitude;      // 纬度
    time_t last_updated; // 最后更新时间
} SensorData;

// 客户端数据结构
typedef struct {
    int fd;                     // 客户端文件描述符
    char buffer[BUFFER_SIZE];   // 接收缓冲区
    ssize_t buffer_len;         // 缓冲区数据长度
    int response_sent;          // 响应是否已发送
} Client;
extern Client clients[MAX_CLIENTS];
extern int client_count;
extern int epoll_fd;
extern int server_fd;
extern volatile sig_atomic_t running;



int set_nonblocking(int fd);
void handle_shutdown(int sig);
int find_client_index(int fd);
int add_client(int fd);
void remove_client(int index);
int file_exists(const char *filename);
int send_http_header(int client_fd, const char *content_type, int content_length, int status_code);
void handle_data_request(int client_fd, int client_index,data_t *data); 
void handle_html_request(int client_fd, int client_index);
void handle_image_request(int client_fd, int client_index);
void process_request(int client_index,data_t *data);
void handle_client_data(int client_index,data_t *data); 
int init_server();
ssize_t send_all(int fd, const void *buf, size_t len);
#endif // !__SERVER_H
