#include "stc8a.h"
//#include "ADC.h"

unsigned long T0Set=0;   //定时器计数
unsigned int SendTickTimerSet=0;		//心跳的定时器
long ConnectErrorTimerSet=60000;		//通讯连接定时器
unsigned int soundTime=100;//0.1s一次
//Dh11的定时器

extern float sound;

sbit Key=P0^3;//这个是设置热点的按键
unsigned int KeyCount=0;
bit SetEspToAPFlag=0;

unsigned int LightFive=5000;
bit lightShortFlag=0;//短暂亮灯标志
sbit LED0=P0^2;//暂时替代继电器
bit forgetLight=0;//忘记关灯标志

bit soundFlag=0;



//定时器0初始化
void Timer0Init(void)		//1毫秒@22.1184MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;   //开启定时器0的中断
}


//定时器0的中断服务程序
void T0Isr() interrupt 1
{
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值
	
	if(Key==0)
	{
		KeyCount++;//如果为5000则到达
		if(SetEspToAPFlag)
		{
			KeyCount=0;//KeyCount重置为0	//防止一次按下10s，重复进行重启
		}
		if(KeyCount>=5000)
		{//5秒后
			KeyCount=0;//KeyCount重置为0
			SetEspToAPFlag=1;//标记置为1
		}
	}
	if(T0Set)T0Set--;
	if(SendTickTimerSet)SendTickTimerSet--;
	if(forgetLight){//忘记关灯，才进行这个判断
		if(LightFive)//亮灯时间大于0
		{
			LightFive--;
		}
		else{
			if(lightShortFlag==1){//说明要亮了
				LightFive=5000;//设置5s
				LED0=0;//让他亮起来
				lightShortFlag=0;
			}
			else{//说明不亮
				LED0=0xff;
			}
		}
	}
	
	//if(ConnectErrorTimerSet)ConnectErrorTimerSet--;
	//设置声敏传感器的检测
//	if(soundTime>0)
//	{
//		soundTime--;
//	}
//	else
//	{
//		//到了0.1s，ad检测一次
//		getSound();
//		if(sound>3)
//		{
//			//说明有声音了，要发送消息告诉服务端,同时加长检测时间
//			soundTime=5000;//5s后再进行检测
//		}
//		else
//		{//没有声音，继续0.1s检测
//			soundTime=100;
//		}
//	}
	
	
}
