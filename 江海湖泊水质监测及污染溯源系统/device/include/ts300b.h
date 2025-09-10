#ifndef __TS300B_H
#define __TS300B_H

int ts300b_get_scale(const char * filepath,float* scale);
int  ts300b_get_data(const char* filepath,int *data);
int  ts300b_get_tur(const char *filepath,float *Turbidity ,float *temp,float *scale);

#endif // !__TS300B_
