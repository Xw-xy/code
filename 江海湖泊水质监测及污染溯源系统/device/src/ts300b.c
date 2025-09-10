#include "../include/ts300b.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int ts300b_get_scale(const char * filepath,float* scale)
{
   int ret = 0;
    char *str = NULL;
    FILE* data_stream;

    data_stream = fopen(filepath,"r");
    if(data_stream == NULL)
    {
        printf("can'no open file %s\r\n",filepath);
        return -1;
    }

    ret = fscanf(data_stream,"%s",str);
    if(!ret)
    {
        printf("file read error!\r\n");
    }else if(ret == EOF)
    {
        fseek(data_stream,0,SEEK_SET);
    }
    *scale =  atof(str);
    fclose(data_stream);
    return 0;


}
int  ts300b_get_data(const char* filepath,int *data)
{
    
    int ret = 0;
    char str[50] = {0};
    FILE* data_stream;

    data_stream = fopen(filepath,"r");
    if(data_stream == NULL)
    {
        printf("can'no open file %s\r\n",filepath);
        return -1;
    }

    ret = fscanf(data_stream,"%s",str);
    if(!ret)
    {
        printf("file read error!\r\n");
    }else if(ret == EOF)
    {
        fseek(data_stream,0,SEEK_SET);
    }
    *data =  atoi(str);
    fclose(data_stream);
    return 0;
}
int  ts300b_get_tur(const char *filepath,float *Turbidity ,float *temp,float *scale)
{
    float vol,vol1,uvol,curtemp,K = 3291.3,tur;
    int ret = 0;
    int adc = 0;
    curtemp = *temp;

    ts300b_get_data(filepath,&adc);
    printf("adc1 = %d\r\n",adc);
    vol =(float) adc*(*scale)/1000; //计算当前电压值
    printf("vol = &f\r\n",vol);
    //uvol = -0.0192*(curtemp - 25);
    //vol1 = vol - uvol;
    //K = 865.8*vol1;
    tur = -865.68*vol + K;
    *Turbidity = tur;

    return 0;
}

