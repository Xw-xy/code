#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include "../include/uart.h"






static struct termios old_cfg;  /*用于保存终端的配置参数*/
int fd;  /* 串口对应的文件描述符*/


/*
*
*串口初始化
*/

 int uart_init(const char *device)
{
    /*打开终端设备*/
    fd = open(device,O_RDWR | O_NOCTTY);
    if(fd < 0)
    {
        fprintf(stderr,"open error:%s:%s\r\n",device,strerror(errno));
        return -1;
    }
    /*获取串口当前配置参数*/
    if(tcgetattr(fd,&old_cfg) < 0)
    {
        fprintf(stderr,"tcgetattr error:%s\n",strerror(errno));
        close(fd);
        return -1;

    }
    return fd;
}


/*
*串口配置
*/

 int uart_cfg(const uart_cfg_t *cfg)
{
    struct termios new_cfg = {0};
    speed_t speed; 

    /*设置为原始模式*/
    cfmakeraw(&new_cfg);

    /*使能接收*/
    new_cfg.c_cflag |= CREAD;


    /*设置波特率*/
    switch(cfg->baudrate)
    {
        case 1200:speed = B1200;
            break;
        case 1800:speed = B1800;
            break;
        case 2400:speed = B2400;
            break;
        case 4800:speed = B4800;
            break;
        case 9600:speed = B9600;
            break;
        case 19200:speed = B19200;
            break;
        case 38400:speed = B38400;
            break;
        case 57600:speed = B57600;
            break;
        case 115200:speed = B115200;
            break;
        case 230400:speed = B230400;
            break;
        case 460800:speed = B460800;
            break;
        case 500000:speed = B500000;
            break;
        default:
            speed = B115200;
            printf("default baud rate:115200\r\n");
            break;
        
    }
    if(0>cfsetspeed(&new_cfg,speed)){
        fprintf(stderr,"cfsetspeed error:%s\r\n",strerror(errno));
        return -1;
    }

    /*设置数据位大小*/
    new_cfg.c_cflag &= ~CSIZE;  //将数据位相关的比特位清零

    switch(cfg->dbit){
        case 5:
            new_cfg.c_cflag |= CS5;
            break;
        case 6:
            new_cfg.c_cflag |= CS6;
            break;
        case 7:
            new_cfg.c_cflag |= CS7;
            break;
        case 8:
            new_cfg.c_cflag |= CS8;
            break;
        default:
            new_cfg.c_cflag |= CS8;
            printf("default data bit size:8\r\n");
            break;

    }

    /*设置奇偶校验位*/
    switch(cfg->parity)
    {
    case 'N':   //无校验
        new_cfg.c_cflag &= ~PARENB;
        new_cfg.c_iflag &= ~INPCK;
        /* code */
        break;
    case 'O':   //奇校验
        new_cfg.c_cflag |= (PARODD|PARENB);
        new_cfg.c_iflag |= INPCK;
        /* code */
        break;
    case 'E':   //偶校验
        new_cfg.c_cflag |= PARENB;
        new_cfg.c_cflag &= ~PARODD;    //清除PARODD标志，配置为偶校验
        new_cfg.c_iflag |= INPCK;
        /* code */
        break;
    default:
        new_cfg.c_cflag &= ~PARENB;
        new_cfg.c_iflag &= ~INPCK;
        break;
    }

    /*设置停止位*/
    switch(cfg->sbit)
    {
        case 1: //一个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            break;
        case 2: //两个停止位
            new_cfg.c_cflag |= CSTOPB;
            break;
        default:    //默认为1个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            break;

    }

    /*将MIN和TIME设置为0*/
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 0;

    /*清空缓冲区*/
    if(tcflush(fd,TCIOFLUSH) < 0){
        fprintf(stderr,"tcflush error:%s\r\n",strerror(errno));
        return -1;
    }
    if (tcsetattr(fd,TCSANOW,&new_cfg) < 0)
    { 
        fprintf(stderr,"tcsetattr error:%s\n",strerror(errno));
        return -1;
        /* code */
    }
    close(fd);
    return 0;
    
}

 void io_handler(int sig,siginfo_t *info,void *contex)
{

    char res = 0;
    int ret = 0;;
    int n;
    if(SIGRTMIN != sig)
    {
        return ;
    }

    if(POLL_IN == info->si_code){

        
    }

       
}

void async_io_init(void)
{
    struct sigaction sigatn;
    int flag;

    /*使能异步IO*/
    flag = fcntl(fd,F_GETFL);
    flag |= O_ASYNC;
    fcntl(fd,F_SETFL,flag);

    /*设置异步IO的所有者*/
    fcntl(fd,F_SETOWN,getpid());
    
    /*指定实时信号SIGRTMIN作为异步IO通知信号*/
    fcntl(fd,F_SETSIG,SIGRTMIN);

    /*为实时信号SIGNRTMIN注册信号处理函数*/
    sigatn.sa_sigaction = io_handler;
    sigatn.sa_flags = SA_SIGINFO;
    sigemptyset(&sigatn.sa_mask);
    sigaction(SIGRTMIN,&sigatn,NULL);

}