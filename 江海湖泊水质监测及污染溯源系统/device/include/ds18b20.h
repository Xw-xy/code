#ifndef __DS18B20_H
#define __DS18B20_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int  da18b20_get_data(int fd,unsigned short* data);
int  ds18b20_get_temp(int fd,float* temp);



#endif // !__DS18B20_H
