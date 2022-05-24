#include "stc8a.h"
#include "Uart.h"
#include "Timer0.h"
#include "string.h"
#include "stdio.h"
#include "FlashIAP.h"


sbit LED0=P2^0;
sbit LED1=P2^1;

extern bit SetEspToAPFlag;
extern char Uart2RecFlag;
extern char xdata Uart2RecBuff[100];
//从Dh11中引入data数据，用于发送
extern char DH11_date[18];

extern unsigned long T0Set;   //定时器计数

char xdata ESP8266RecBuf[100];  //ESP8266数据缓存

extern long ConnectErrorTimerSet;//通讯连接定时器

extern bit SendTickTimerFlag;
extern bit ConnectErrorTimerFlag;


char xdata FlashBuff[100];
char xdata APName[50];
char xdata APPassword[50];




//发AT指令给ESP8266函数
//参数1：待发送字符串   参数2 超时时间  参数3 正常返回的字符串
//返回值， 1--发送成功   -1--表示发送失败
char SendATTo8266(char *ATStr,long OutTime,char *RetStr)
{
	//发出指令
	Uart2SendString(ATStr);
	Uart2RecFlag=0;
	//等待串口2回复
	while(1)
	{
		T0Set=OutTime; //设置超时时间
		while(Uart2RecFlag==0 && T0Set!=0); //等待数据
		if(Uart2RecFlag==1)
		{//没有超时，正常接收到数据
			Uart2RecFlag=0;
			if(strcmp(Uart2RecBuff,RetStr)==0)
			{//接收到复位完成数据
				return 1;
			}
		}
		else if(Uart2RecFlag==3)
		{//没有超时，收到>
			Uart2RecFlag=0;
			return 1;
		}
		else if(T0Set==0)return -1;  //超时返回
		else if(strcmp(Uart2RecBuff,"ERROR\r\n")==0)
		{//收到的时ERROR，返回
			return -1;
		}
	}
}

//向通讯对方发送字符串
//利用AT+CIPSEND指令
//参数1：通道号  参数2：发送的数据
//返回值，-1 表示发送失败，1 表示发送成功
char ESP8266SendData(int Channel,char *strdata)
{
	char xdata stemp[25];
	unsigned int len=strlen(strdata);  //获取数据长度
	//组合 AT+CIPSEND=Channel,len
	memset(stemp,0,25*sizeof(char));
	sprintf(stemp,"AT+CIPSEND=%d,%d\r\n",Channel,len);  // AT+CIPSEND=Channel,len\r\n
	if(SendATTo8266(stemp,5000,">")==1)
	{//发送成功 并且收到>号，需要发送后续数据
		if(SendATTo8266(strdata,10000,"SEND OK\r\n"))
		{//发送后续字符串正常结束
			return 1;
		}
		else return -1;
	}
	return -1;
}

//读取Flahs中的AP
char ReadAPFromFlash()
{
	char i;
	memset(FlashBuff,0,100*sizeof(char));//热点名:密码
	FlashRead(0,FlashBuff);
	//替换:为空格
	i=0;
	while(FlashBuff[i]!=':'&&FlashBuff[i]!=0)i++;
	if(FlashBuff[i]==0) return 0;
	FlashBuff[i]=' ';
	//分隔字符串
	memset(APName,0,50*sizeof(char));
	memset(APPassword,0,50*sizeof(char));
	i=sscanf(FlashBuff,"%s %s",APName,APPassword);
	if(i!=2) return 0;
	else 
	{
		return 1;
	}
}

//ESP8266初始化
char ESP8266Init()
{
	char stepflag=0;
	while(1)
	{
		//发出复位指令
		if(SetEspToAPFlag){//设置ap模式
			break;//跳出循环
		}
		if(stepflag==0)
		{
			SendATTo8266("AT+CIPCLOSE=0 \r\n\0",2000,"OK\r\n\0");
			if(SendATTo8266("AT+RST\r\n\0",2000,"ready\r\n")==1)
			{
				stepflag++;
			}
		}
		else if(stepflag==1)
		{//发出AT指令
			if(SendATTo8266("AT\r\n\0",100,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==2)
		{//发出AT+CWMODE=2指令--设置8266为STA模式
			if(SendATTo8266("AT+CWMODE=1\r\n\0",500,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==3)
		{
			if(ReadAPFromFlash()==1)
			{//读取成功
				LED0=0;
				memset(FlashBuff,0,100*sizeof(char));
				sprintf(FlashBuff,"AT+CWJAP=\"%s\",\"%s\"\r\n\0",APName,APPassword);	
			}
			else
			{//读取失败
				LED1=0;
				memset(FlashBuff,0,100*sizeof(char));
				//sprintf(FlashBuff,"AT+CWJAP=\"yikolemon\",\"88888888\"\r\n\0");	
				sprintf(FlashBuff,"AT+CWJAP=\"computer\",\"123456789\"\r\n\0");	
			}
			//发出AT+CWSAP="yyds","12345678",1,3 指令--设置8266热点名称
			if(SendATTo8266(FlashBuff,30000,"OK\r\n")==1)
			//if(SendATTo8266("AT+CWJAP=\"yikolemon\",\"88888888\"\r\n\0",180000,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else 
			{
				stepflag=0;
			}
		}
		else if(stepflag==4)
		{//发出AT+CIPMUX=1 指令--设置为多路模式
			if(SendATTo8266("AT+CIPMUX=1\r\n\0",500,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==5)
		{//发出AT+CIPSERVER=1,8100 指令--开启服务器
			//if(SendATTo8266("AT+CIPSTART=0,\"TCP\",\"114.115.237.170\",8000\r\n\0",10000,"OK\r\n")==1)
			//if(SendATTo8266("AT+CIPSTART=0,\"TCP\",\"10.109.236.75\",5001\r\n\0",10000,"OK\r\n")==1)
			if(SendATTo8266("AT+CIPSTART=0,\"TCP\",\"192.168.137.1\",7777\r\n\0",10000,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==6){
			//ESP8266SendData(0,FlashBuff);
			if(ESP8266SendData(0,"2120191799 duanfuqiang\n")==-1)
			{
				stepflag=0;
			}
			else//消息发送成功
			{
				//ConnectErrorTimerSet=180000;
				ConnectErrorTimerSet=60000;//重置超时连接时间
				SendTickTimerFlag=1;		//打开心跳
				ConnectErrorTimerFlag=1;//打开连接错误检测
				return 1;
			}
		}
	}
}	



	//ESP8266——AP初始化
char ESP8266Init_AP()
{
	char stepflag=0;
	while(1)
	{
		//发出复位指令
		if(stepflag==0)
		{
			if(SendATTo8266("AT+RST\r\n\0",2000,"ready\r\n")==1)
			{
				stepflag++;
			}
		}
		else if(stepflag==1)
		{//发出AT指令
			if(SendATTo8266("AT\r\n\0",100,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==2)
		{//发出AT+CWMODE=2指令--设置8266为AP模式
			if(SendATTo8266("AT+CWMODE=2\r\n\0",500,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==3)
		{//发出AT+CWSAP="yyds","12345678",1,3 指令--设置8266热点名称
			if(SendATTo8266("AT+CWSAP=\"yikolemon_8266\",\"12345678\",1,3\r\n\0",30000,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==4)
		{//发出AT+CIPMUX=1 指令--设置为多路模式
			if(SendATTo8266("AT+CIPMUX=1\r\n\0",500,"OK\r\n")==1)
			{//发送正常
				stepflag++;
			}
			else stepflag=0;
		}
		else if(stepflag==5)
		{//发出AT+CIPSERVER=1,8100 指令--开启服务器
			if(SendATTo8266("AT+CIPSERVER=1,8200\r\n\0",5000,"OK\r\n")==1)
			{//发送正常
				SendTickTimerFlag=0;		//关闭心跳
				ConnectErrorTimerFlag=0;//关闭连接错误检测
				return 1;
			}
			else stepflag=0;
		}
	}
}
