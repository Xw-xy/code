#include "../include/ds18b20.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

int tds_get_scale(const char * filepath,float* scale)
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
    *scale =  atof(str);
    fclose(data_stream);
    return 0;

}

int  tds_get_data(char* filepath,int  *data)
{
    int ret = 0;
    char str[50] = {0};
    FILE* data_stream;

    data_stream = fopen(filepath,"r");
    if(data_stream == NULL)
    {
        printf("can't open file %s\r\n",filepath);
        return -1;
    }

    ret = fscanf(data_stream,"%s",str);
    if(!ret)
    {
        perror("file read error!\r\n");
    }else if(ret == EOF)
    {
        fseek(data_stream,0,SEEK_SET);
    }
    *data =  atoi(str);
    fclose(data_stream);
    return 0;
}
int  tds_get_cond( char *filepath ,float *conductivity ,float *temp,float *scale)
{
    
    float voltage = 0,curtemp = 0;
    float tds_test = 0;
    int ret = 0;
    int adc = 0;
    tds_get_data(filepath,  &adc);
    printf("adc = %d\r\n",adc);

    curtemp = 1 + 0.02*(*temp - 25);
    voltage =(float) (curtemp * adc * ( * scale)/1000);
    tds_test = (float)((66.71 * voltage * voltage * voltage - 127.93 * voltage * voltage +428.7 * voltage ));
    *conductivity = tds_test;

    return 0;
}

