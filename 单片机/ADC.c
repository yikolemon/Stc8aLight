#include "stc8a.h"
#include "stdio.h"
#include "intrins.h"

unsigned int dat;
float ftemp;
char light[10];
//char sound[10];
bit sound;

void getLight()
{
	
    P1M0 = 0x00;                            //设置P1.2为高祖输入模式，1.2用光敏电阻
    P1M1 = 0x04;
		
    ADCCFG = 0x2f;                          //设置ADC时钟为系统时钟/2/16/16
    ADC_CONTR = 0x80;                       //使能ADC模块

		ADC_CONTR |= 0x42;                      //启动AD转换 ，P1.2为adc口
		_nop_();
		_nop_();
		while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
		ADC_CONTR &= ~0x20;                     //清完成标志
		//ADC_RES(高4位) ADC_RESL（低8位）;     //读取ADC结果
		dat=ADC_RES*256+ADC_RESL;
		ftemp=dat;
		ftemp=ftemp*3.3/256;
		///4096
		sprintf(light,"L:%.2f\r\n\0",ftemp);
		//light为结果
	
}

void getSound()
{
	
		P1M0 = 0x00;                                
		P1M1 = 0x08;														//设置P1.3为高祖输入模式，1.3用作声控传感器

    ADCCFG = 0x2f;                          //设置ADC时钟为系统时钟/2/16/16
    ADC_CONTR = 0x80;                       //使能ADC模块

		ADC_CONTR |= 0x43; 											//启动AD转换 ，P1.3为adc口
		_nop_();
		_nop_();
		while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
		ADC_CONTR &= ~0x20;                     //清完成标志
		//ADC_RES(高4位) ADC_RESL（低8位）;     //读取ADC结果
		dat=ADC_RES*256+ADC_RESL;
		ftemp=dat;
		ftemp=ftemp*3.3/256;
		if(ftemp>8)
		{
			sound=1;
		}
		///4096
		//sprintf(sound,"S:%.2f\r\n\0",ftemp);
		//sound为结果
		
}
	
