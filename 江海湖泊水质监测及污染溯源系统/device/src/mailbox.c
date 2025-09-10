#include "../include/mailbox.h"

struct list_head *mailbox_create(void)
{
    struct list_head *phead = NULL;
    
    phead = malloc(sizeof(struct list_head));
    if (NULL == phead)
    {
        perror("fail mailbox_create");
        return NULL;
    }
    INIT_LIST_HEAD(phead);

    return phead;
}

int mailbox_addtask(struct list_head *ptaskthreadhead,pthread_t tid,char *name,void *(*pfun)(void *))
{
    taskthread_t *ptaskthread = NULL;
    int ret = 0;

    ptaskthread = malloc(sizeof(taskthread_t));
    if (NULL == ptaskthread)
    {
        perror("fail taskthread_create");
        return -1;
    }
    ptaskthread->tid = tid;
    strncpy(ptaskthread->name,name,sizeof(ptaskthread->name));
    ptaskthread->pfun = pfun;
    ptaskthread->ptaskmsghead = malloc(sizeof(struct list_head));
    if (NULL == ptaskthread->ptaskmsghead)
    {
        perror("fail taskmsg_create");
        free(ptaskthread);
        return -1;
    }
    INIT_LIST_HEAD(ptaskthread->ptaskmsghead);
    sem_init(&ptaskthread->sem_msgnum,0,0);

    ret = pthread_create(&ptaskthread->tid,NULL,ptaskthread->pfun,NULL);
    if (ret != 0)
    {
        perror("fail pthread_create");
        sem_destroy(&ptaskthread->sem_msgnum);
        free(ptaskthread->ptaskmsghead);
        free(ptaskthread);
        return -1;
    }

    list_add_tail(&ptaskthread->node,ptaskthreadhead);
    
    return 0;
}   

int mailbox_msgsend(struct list_head *ptaskthreadhead,char *recvername,data_t data)
{
    taskthread_t *ptmptaskthread = NULL;
    taskmsg_t *ptaskmsg = NULL;
    int is_exit = 0;

    list_for_each_entry(ptmptaskthread,ptaskthreadhead,node)
    {
        if (0 == strcmp(ptmptaskthread->name,recvername))
        {
            is_exit = 1;
            break;
        }
    }

    if (0 == is_exit)
    {
        return -1;
    }

    ptaskmsg = malloc(sizeof(taskmsg_t));
    if (NULL == ptaskmsg)
    {
        perror("fail taskmsg_create");
        return -1;
    }
    ptaskmsg->data.conductivity = data.conductivity;
    ptaskmsg->data.latitude = data.latitude;
    ptaskmsg->data.longitude= data.longitude;
    ptaskmsg->data.temperature = data.temperature;
    ptaskmsg->data.turbidity = data.turbidity;
    #if 0
    ptaskmsg->data.temp = data.temp;
    ptaskmsg->data.voltage = data.voltage;
    ptaskmsg->data.current = data.current;
    ptaskmsg->data.resistor = data.resistor;
    #endif

   list_add_tail(&ptaskmsg->node,ptmptaskthread->ptaskmsghead);
   sem_post(&ptmptaskthread->sem_msgnum);

   return 0;
}

int mailbox_msgrecv(struct list_head *ptaskthreadhead,data_t *data)
{
    taskthread_t *ptmpthread = NULL;
    taskmsg_t *ptmpmsg = NULL;
    int is_exit = 0;

    list_for_each_entry(ptmpthread,ptaskthreadhead,node)
    {
        if (ptmpthread->tid == pthread_self())
        {
            is_exit = 1;
            break;
        }
    }

    if (0 == is_exit)
    {
        return -1;
    }

    //while (list_empty(ptmpthread->ptaskmsghead));
    sem_wait(&ptmpthread->sem_msgnum);

    ptmpmsg = list_entry(ptmpthread->ptaskmsghead->next,taskmsg_t,node);
    data-> conductivity= ptmpmsg->data.conductivity;
    data-> latitude= ptmpmsg->data.latitude;
    data-> longitude= ptmpmsg->data.longitude;
    data-> temperature= ptmpmsg->data.temperature;
    data-> turbidity= ptmpmsg->data.turbidity;
    #if 0
    data->voltage = ptmpmsg->data.voltage;
    data->current = ptmpmsg->data.current;
    data->resistor = ptmpmsg->data.resistor;
    #endif
    list_del(&ptmpmsg->node);
    free(ptmpmsg);

    return 0;
}

int mailbox_destroy(struct list_head *ptaskthreadhead)
{
    taskthread_t *pfreethread = NULL;
    taskthread_t *pnextthread = NULL;
    taskmsg_t *pfreemsg = NULL;
    taskmsg_t *pnextmsg = NULL;

    list_for_each_entry_safe(pfreethread,pnextthread,ptaskthreadhead,node)
    {
        sem_destroy(&pfreethread->sem_msgnum);
        list_for_each_entry_safe(pfreemsg,pnextmsg,pfreethread->ptaskmsghead,node)
        {
            list_del(&pfreemsg->node);
            free(pfreemsg);
        }
        list_del(&pfreethread->node);
        free(pfreethread);
    }

    return 0;
}