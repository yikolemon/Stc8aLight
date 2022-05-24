#ifndef __ESP8266-2__H__
#define __ESP8266-2__H__

//串口2用到的变量
extern bit Uart2SendFinishFlag;   //串口2发送字符完成标志  0--表示已经完成 1--表示未完成
extern char xdata Uart2RecBuff[100];  //串口2的接收缓存
extern char Uart2RecCount;          //串口2的接收计数，最大100
extern char Uart2RecFlag;            //串口2接收标记 1--表示接收到数据，数据放在缓存里，数据长度为Uart2RecCount
extern unsigned int T0Set;   //定时器计数

extern char xdata ESP8266RecBuf[100];  //ESP8266数据缓存

#endif
