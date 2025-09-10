#ifndef     __MAILBOX_H__
#define     __MAILBOX_H__

#include "list.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

typedef struct {
    float temperature;   // 温度 (℃)
    float conductivity;  // 电导率 (μS/cm)
    float turbidity;     // 浑浊度 (NTU)
    float ph;            // pH值
    float longitude;     // 经度
    float latitude;      // 纬度
    //time_t last_updated; // 最后更新时间
}data_t;//数据类型，提高代码复用性

typedef struct taskthread{
    struct list_head node;
    pthread_t tid;//任务线程的id号
    char name[32];//任务线程的名字
    void *(*pfun)(void *);//任务线程入口函数地址
    sem_t sem_msgnum;
    struct list_head *ptaskmsghead;//消息的头结点
}taskthread_t;

typedef struct taskmsg{
    struct list_head node;
    char sender[32];//发送者
    data_t data;//数据
}taskmsg_t;

extern struct list_head *mailbox_create(void);
extern int mailbox_addtask(struct list_head *ptaskthreadhead,pthread_t tid,char *name,void *(*pfun)(void *));
extern int mailbox_msgsend(struct list_head *ptaskthreadhead,char *recvername,data_t data);
extern int mailbox_msgrecv(struct list_head *ptaskthreadhead,data_t *data);
extern int mailbox_destroy(struct list_head *ptaskthreadhead);

#endif
