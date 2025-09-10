#include "../include/ds18b20.h"



int da18b20_get_data(int fd,unsigned short* data)
{
    int ret = 0;
    ret = read(fd, data ,2);
    if(ret < 0)
    {
        printf("fail read data of ds18b20\r\n");
        return -1;
    }
    return 0;

}
int ds18b20_get_temp(int fd,float* temp)
{
    unsigned short org_databuf;
    float cur;
    int ret = 0;
    ret = da18b20_get_data( fd,&org_databuf); 
    if(ret < 0)
    {
        perror("fail read data of ds18b20:");
        return -1;
    }
    if(((org_databuf >> 11) & 0x1f))
    {
        cur = (float)(~org_databuf)*0.0625;
    }
    else{
        cur = (float)(org_databuf*0.0625);
    }
    *temp = cur;
    return 0;

}
