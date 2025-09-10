#ifndef __TDS_H
#define __TDS_H

int tds_get_scale(const char * filepath,float* scale);
int  tds_get_data( const char* filepath,int  *data);
int  tds_get_cond( const char *filepath ,float *conductivity ,float *temp,float *scale);


#endif // DEBUG
