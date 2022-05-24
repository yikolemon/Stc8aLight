#ifndef __ESP__H__
#define __ESP__H__

char SendATTo8266(char *ATStr,int OutTime,char *RetStr);
char ESP8266SendData(int Channel,char *strdata);
char ESP8266Init();

#endif