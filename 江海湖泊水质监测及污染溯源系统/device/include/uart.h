#ifndef __UART_H
#define __UART_H





typedef struct uart_cfg
{
    unsigned int baudrate; /*波特率*/
    unsigned char dbit;     /*数据位*/
    char parity;            /*奇偶校验位*/
    unsigned char sbit;     /*停止位*/
}uart_cfg_t;


 void async_io_init(void);
 int uart_cfg(const uart_cfg_t *cfg);
 int uart_init(const char *device);
#endif // !__UART_H
