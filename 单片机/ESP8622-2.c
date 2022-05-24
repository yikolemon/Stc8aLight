#include "stc8a.h"
#include <intrins.h>   //使用其nop()进行延时
#include <string.h>
#include "stdio.h"
#include "Uart.h"
#include "Timer0.h"
#include "ESP.h"
#include "ESP8622-2.h"
#include "stdlib.h"
#include "FlashIAP.h"
#include "Dh11.h"
#include "ADC.h"



//=========5.19=========

sbit LED0=P0^2;//暂时替代继电器
extern bit lightShortFlag;
//=======================

#define SendTickSet 15000;//15秒

extern unsigned int SendTickTimerSet;		//心跳的定时器
extern long ConnectErrorTimerSet;		
extern bit soundFlag;
extern bit forgetLight;//没关灯标志

//从Dh11中引入data数据，用于发送
extern char DH11_date[20];
//从ADC中引入数据，用于发送
extern char light[10];
//extern char sound[10];
extern bit sound;

extern bit SetEspToAPFlag;

bit SendTickTimerFlag=1;
bit ConnectErrorTimerFlag=1;
//======================



char ansComm(char *Str,int *CommNo,char *CommDat)
{
	char xdata Head[5];
	//遍历字符串 替换，计算BCC值
	int i=0;
	int itemp=0; //BCC计算
	int Count=0; //统计，的数量
	int BccDat=0;
	while(Str[i]!=0)
	{
		if(Str[i]==',')
		{
			Count++;
			itemp^=Str[i];
			Str[i]=' ';
		}
		else if(Count<3)
		{
			itemp^=Str[i];
		}
		i++;
	}
	i=sscanf(Str,"%s %d %s %d;",Head,CommNo,CommDat,&BccDat);
	if(i==4)
	{
		if(strcmp(Head,"NJUE")==0)
		{
			if(itemp==BccDat)
			{
				return 1;
			}
		}
	}
	return 0;
}



main()
{
	
	
	int i=0,itemp=0;
	int No,Timer;
	int CommNo;
	char xdata CommDat[20];
	
	Uart1Init();
	Uart2Init();
	Timer0Init();
	EA=1;
	
	//5.19	Delay1500ms();  //DHT11上电后要等待1.5S以越过不稳定状态在此期间不能发送任何指令
	Delay1500ms();  //DHT11上电后要等待1.5S以越过不稳定状态在此期间不能发送任何指令
	ESP8266Init();	
	
	P0M0 = 0x04;                                //设置P0.2为推挽输出
	P0M1 = 0x00;
	
	
	while(1)
	{
		//=========5.19===========
//		if(Dh11Flag){
//			DHT11_receive();
//			ESP8266SendData(0,DH11_date);
//			Dh11Flag=0;
//			//发送光照数据
//		}
		//ESP8266SendData(0,);
		//=========================
//		if(soundFlag){
//			ESP8266SendData(0,"有声音了，注意/r/n");
//			soundFlag=0;
//		}
		getSound();
		if(sound)//检测到声音
		{
			ESP8266SendData(0,"this is sound\r\n");
			sound=0;
		}
		if(Uart2RecFlag==2)
		{//接收到数据
			Uart2RecFlag=0;
			while(!TI);
			TI=0;	
			memset(ESP8266RecBuf,0,100*sizeof(char));
			strcpy(ESP8266RecBuf,Uart2RecBuff);
			//让P2口亮起发送来的数据
			if(strcmp(ESP8266RecBuf,"TICK\r\n\0")!=0)//判断是不是TICK,如果相等返回0,这下面表示这不是TICK
			{
				i=ansComm(ESP8266RecBuf,&CommNo,CommDat);
				if(i==1)
				{//解析成功
					switch(CommNo)
					{
						case 1:
						{//点亮led更改为获取温湿度
							//itemp=atoi(CommDat);//提取编号不需要
//							switch(itemp)
//							{
								DHT11_receive();
								ESP8266SendData(0,DH11_date);
							//}
								break;
						}
						case 2:
						{//灭掉led
							//itemp=atoi(CommDat);//提取编号
							//switch(itemp)
							//{
							//}
							getLight();
							ESP8266SendData(0,light);
							LED0=0;
							break;
						}
						case 3:
						{//设置继电器状态
							itemp=atoi(CommDat);//提取编号
							switch(itemp)
							{
								case 1:
								{//设置为通路 NJUE,3,1,58;
									forgetLight=0;
									P0M0 = 0x04;                                //设置P0.2为推挽输出
									P0M1 = 0x00;
									LED0=0;
									break;
								}
								case 2:
								{//设置为断路 NJUE,3,2,57;
									forgetLight=1;
									P0M0 = 0x04;                                //设置P0.2为推挽输出
									P0M1 = 0x00;
									LED0=0xff;
									break;
								}
							}
							break;
						}
						case 4:
						{//设置热点
							FlashWrite(0,Com?mDat);
							ESP8266Init();
							//memset(APName,0,30*sizeof(char));
							//FlashRead(0,APName);
							//ESP8266SendData(0,APName);
							break;
						}
						case 5:
						{//设置亮灯5s
							lightShortFlag=1;//设置短暂亮灯标志
							break;
						}
						break;
					}
				}
				else
				{
					ESP8266SendData(0,"Error\r\n\0");
				}
				//P2=~i;
			}
			else//收到的是TICK，进行更新Tick计时
			{
				ConnectErrorTimerSet=60000;
			}
			//显示数据
//			i=0;
//			while(ESP8266RecBuf[i]!=0) //12345
//			{
//				SBUF=ESP8266RecBuf[i];
//				while(!TI);
//				TI=0;
//				i++;
//			}
//			i=strlen(ESP8266RecBuf);  //字符串长度   12345678  8
//			ESP8266RecBuf[i-1]++;     //最后一个字节内容加1
//			ESP8266SendData(0,ESP8266RecBuf);
		}
		
		
		if(SendTickTimerSet==0 && SendTickTimerFlag)
		{//心跳消息发送
			memset(ESP8266RecBuf,0,100*sizeof(char));
			strcpy(ESP8266RecBuf,"TICK\r\n");
			if(ESP8266SendData(0,ESP8266RecBuf)==-1)
			{//发送失败
				ESP8266Init();
			}
			//ESP8266SendData(0,"shit\n");
			else SendTickTimerSet=SendTickSet;
		}
		
		if(ConnectErrorTimerSet==0 && ConnectErrorTimerFlag)
		{//3分钟未收到服务器数据
			ESP8266Init();
		}
		
		if(SetEspToAPFlag)
		{//设置热点按键长按下
			SetEspToAPFlag=0;
			ESP8266Init_AP();
		}
	}
}