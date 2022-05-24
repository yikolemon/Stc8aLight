#include "stc8a.h"
#include "stdio.h"
#include "intrins.h"
#include "math.h"

#define uchar unsigned char 
#define uint unsigned int
	
uchar DH11_date[20];   //用于显示的接收数据数组

	
//测试工作频率为22.1184MHz
	
bit Uar2SendFlag=0;
sbit Data = P3^6;//定义数据线
unsigned char temp;
unsigned char checknum;
unsigned char loopnum=1;
	
//==============延迟10ms===========
void Delay10ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 2;
	j = 32;
	k = 60;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}




//==============延迟1000ms===========
void Delay1000ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 113;
	j = 53;
	k = 228;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}





//==============延迟1500ms===========
void Delay1500ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 169;
	j = 80;
	k = 87;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



//==============延时3us================
void Delay3us()		//@22.1184MHz
{
	unsigned char i;

	i = 20;
	while (--i);
}



//==============延时5us================
void Delay5us()		//@22.1184MHz
{
	unsigned char i;

	i = 35;
	while (--i);
}


//==============延时7us================
void Delay7us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 49;
	while (--i);
}



//==============延时8us================
void Delay8us()		//@22.1184MHz
{
	unsigned char i;

	i = 57;
	while (--i);
}


//==============延时10us================
void Delay10us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 71;
	while (--i);
}

//==============延时20us================
void Delay20us()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	i = 145;
	while (--i);
}



//==============DHT11开始信号=============
             
void DHT11_start()
{
	Data=0;
	Delay10ms();   //延时18ms以上
	Delay10ms();
	Data=1;
	Delay10us();
	Delay3us();
}

//=============接收八位二进制================            
uchar DHT11_rec_byte()      //接收一个字节
{
	uchar i,dat=0;
	for(i=0;i<8;i++)    //从高到低依次接收8位数据
	{          
		while(!Data);
		Delay20us();
		Delay10us(); 		//延时30us，如果还为高则数据为1，否则为0 ，因为27-28us高电平为0，70us为1，所以在30us时
		dat<<=1;        //移位使正确接收8位数据，数据为0时直接移位
		if(Data==1)    			 //dh11一位发送结束，将接收的一位判断计入dat中
		{
			dat+=1;
		}
		while(Data);    //等待数据线拉低    
	}
	return dat;
}

//=============DH11接收40bit数据==================
void DHT11_receive()      //接收40位的数据
{
	unsigned char R_H,R_L,T_H,T_L,RH,RL,TH,TL,revise; 
	DHT11_start();			//调用DHT11_start函数，使Dh11从休眠准备阶段转化到运行状态
	if(Data==0)
	{
		while(Data==0);   //等待拉高     
		Delay20us();
		Delay20us();
		Delay20us();
		Delay20us();
		Delay7us();
	//拉高后延时80us
		R_H=DHT11_rec_byte();    //接收湿度高八位  
		R_L=DHT11_rec_byte();    //接收湿度低八位  
		T_H=DHT11_rec_byte();    //接收温度高八位  
		T_L=DHT11_rec_byte();    //接收温度低八位
		revise=DHT11_rec_byte(); //接收校正位
	 
		Delay20us();
		Delay5us();
	
		if((R_H+R_L+T_H+T_L)==revise)      //校正
		{
			RH=R_H;
			RL=R_L;
			TH=T_H;
			TL=T_L;
		}
		/*数据处理，方便显示*/
		//char数组存储Dh11收集的数据
		DH11_date[0]='0'+(RH/10);
		DH11_date[1]='0'+(RH%10);
		DH11_date[2] ='.';
		DH11_date[3]='0'+(RL/10);
		DH11_date[4]='0'+(RL%10);
		DH11_date[5] = 'R' ;
		DH11_date[6] = 'H';
		DH11_date[7]=' ';
		DH11_date[8]=' ';
		DH11_date[9]=' ';
		DH11_date[10]=' ';
		DH11_date[11]='0'+(TH/10);
		DH11_date[12]='0'+(TH%10);
		DH11_date[13] ='.';
		DH11_date[14]='0'+(TL%10);
		DH11_date[15]='0'+(TL/10);
		DH11_date[16] = 'C';
		DH11_date[17] = '\r';
		DH11_date[18] = '\n';
		DH11_date[19] = '\0';
	}
}