#include "stc8a.h"
#include "string.h"

//串口2用到的变量
bit Uart2SendFinishFlag=0;   //串口2发送字符完成标志  0--表示已经完成 1--表示未完成
char xdata Uart2RecBuff[100];  //串口2的接收缓存
char Uart2RecCount=0;          //串口2的接收计数，最大100
char Uart2RecFlag=0;            //串口2接收标记 1--表示接收到数据，数据放在缓存里，数据长度为Uart2RecCount
char Uart2RecType=0;         //接收模式，为0--正常模式 以\n为结束，>1为特殊模式，专门用于接收+IPD开头的数据
char Uart2RecStepFlag=0;      //特殊接收模式下的步骤变量
char Uart2RecNextCount=0;     //特殊模式下后续字符长度

extern long ConnectErrorTimerSet;

void Uart1Init(void)		//57600bps@22.1184MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFE;		//设定定时初值
	TH1 = 0xFE;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES=1;      //开启串口1中断
}

void Uart2Init(void)		//9600bps@22.1184MHz
{
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xC0;		//设定定时初值
	T2H = 0xFD;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	IE2=ES2;     //开启串口2中断
}

//串口1的中断服务程序
void Uart1ISR() interrupt 4
{
	char temp;
	if(TI)
	{//发送完成中断
		TI=0;
	}
	
	if(RI)
	{//接收完成中断
		RI=0;
		temp=SBUF;
		S2BUF=temp;  //将接收的数据发送给串口2
	}
}

//串口2的中断服务程序
void Uart2ISR() interrupt 8
{
	char temp;
	unsigned char itemp=0;
	char i;
	if(S2CON & 0x02)
	{//发送完成中断
		S2CON &= ~0x02;
		Uart2SendFinishFlag=0;
	}
	
	if(S2CON & 0x01)  //xxxx  xx TI RI
	{//接收中断
		S2CON &= ~0x01; //~0x01= ~(0000 0001)=1111 1110 =0xfe
		temp=S2BUF;
		SBUF=temp; //将接收的数据发送给串口1
		
		//将数据放入缓存
		Uart2RecBuff[Uart2RecCount]=temp;  //将接收到的数据放入缓存
		Uart2RecCount++;  
		//判断接收模式
		if(Uart2RecType==0 && Uart2RecCount==1 && temp=='+')
		{//特殊接收模式
			Uart2RecType=1;
		}
		else if(Uart2RecType==0 && Uart2RecCount==1 && temp=='>')
		{//准备发送字符串模式
			Uart2RecType=2;
		}
		
		if(Uart2RecCount>=100)Uart2RecCount=0;      //当计数到达100，清0
		
		if(Uart2RecType==0)
		{//正常接收模式
			//判断是否接收到换行字符？
			if(temp=='\n')
			{//接到换行
				Uart2RecFlag=1;  //将接收标记置1
				Uart2RecBuff[Uart2RecCount]=0;  //标记接收字符串结束
				if(strcmp(Uart2RecBuff,"0,CLOSED\r\n")==0)
				{//断开连接消息
					ConnectErrorTimerSet=0;
				}
				Uart2RecCount=0;
			}
		}
		else if(Uart2RecType==1)
		{//非正常模式，需要根据长度来判断是否结束
			//+IPD的字符串  +IPD,0,1234:qwr
			//步骤分成3个大步骤
			//1.存储从+号到：的字符串
			//2.从：开始，往前找，号，将该，号到：之间的数值取出，并转换为10进制数，数就是后续字节长度设定计数值
			//3.每接收到一个字符，放入缓存，并将长度计数值减1，直到为0结束
			if(Uart2RecStepFlag==0)
			{//步骤1
				if(temp==':')
				{
					i=Uart2RecCount;  //获取当前位置 --：后的一个位置
					while(Uart2RecBuff[i]!=',')i--;  //找前面的，号
					i++;  //向后移动一位，指示到数字位置
					itemp=0; //数值清0
					//将后续字符串转换为10进制数 atoi()
					while(Uart2RecBuff[i]!=':')
					{
						itemp=itemp*10;
						itemp=itemp+(Uart2RecBuff[i]-0x30);
						i++;
					}
					Uart2RecNextCount=itemp;  //保存后续字符串长度
					//SBUF=Uart2RecNextCount+0x30;
					Uart2RecCount=0; 
					Uart2RecStepFlag++;
				}
			}
			else if(Uart2RecStepFlag==1)
			{//步骤2 接收后续字符 字符个数是Uart2RecNextCount个
				Uart2RecNextCount--;
				if(Uart2RecNextCount==0)
				{//后续字符接收完毕
					Uart2RecBuff[Uart2RecCount]=0;  //字符串截断
					Uart2RecFlag=2;  //将接收标记置2
					ConnectErrorTimerSet=60000;
					Uart2RecStepFlag=0;
					Uart2RecType=0; 
					Uart2RecCount=0;
				}	
			}
		}
		else if(Uart2RecType==2)
		{//首字符接收到>号
			Uart2RecFlag=3;
			Uart2RecType=0; 
			Uart2RecCount=0;
		}
	}
}

//单片机向串口2发送字符串
void Uart2SendString(char *str)
{
	char i=0;
	while(str[i]!=0 && i<100)
	{
		S2BUF=str[i];
		Uart2SendFinishFlag=1;
		while(Uart2SendFinishFlag==1);
		i++;
	}
}