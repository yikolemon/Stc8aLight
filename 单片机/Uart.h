#ifndef __UART_H_   //声明一个编译控制，来使.h文件只声明一次
#define __UART_H_

void Uart1Init(void);		//57600bps@22.1184MHz
void Uart2Init(void);		//9600bps@22.1184MHz
void Uart2SendString(char *str); //单片机向串口2发送字符串
#endif