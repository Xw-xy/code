#include "../include/gps.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include/uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


char  USART_RX_BUF[USART_REC_LEN]; 
unsigned short point1;
_SaveData Save_Data;



void parseGpsBuffer(Data * Data)
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		printf("**************\r\n");
		printf(Save_Data.GPS_Buffer);

		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					printf("Error"); //解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Data->UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Data->latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Data->N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Data->longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Data->E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					printf("Error");	//解析错误
				}
			}


		}
	}
}
void gps_get_data(const char *filepath,Data *data)
{

    int fd = 0;
    int ret;
    char res;
    fd = open(filepath,O_RDWR | O_NOCTTY);
    if(fd< 0)
    {
        printf("open fail\r\n");
        return ;

    }
    while(1)
    {
        ret = read(fd,&res,1);
        if(ret < 0)
        {
            perror("fail read uart:");
            return;
        }
        if(res == '$')
	    {
		    point1 = 0;	
	    }

	    USART_RX_BUF[point1++] = res;

	    if(USART_RX_BUF[0] == '$' && USART_RX_BUF[4] == 'M' && USART_RX_BUF[5] == 'C')			//确定是否收到"GPRMC/GNRMC"这一帧数据
	    {
		    if(res == '\n')									   
		    {
			    memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
			    memcpy(Save_Data.GPS_Buffer, USART_RX_BUF, point1); 	//保存数据
			    Save_Data.isGetData = true;
			    point1 = 0;
			    memset(USART_RX_BUF, 0, USART_REC_LEN);      //清空		
                break;			
		    }
		
	    }

        if(point1 >= USART_REC_LEN)
	    {
		    point1 = USART_REC_LEN;
	    }

       
    }	
    parseGpsBuffer(data);
    close(fd);


}

