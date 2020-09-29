#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f4xx.h"
#include "sys.h"


/*
*以下参数需要用户自行修改才能测试用过
*/
#define User_ESP8266_SSID     "TPlink-733"          //wifi名
#define User_ESP8266_PWD      "dianzixi733"      //wifi密码

#define User_ESP8266_TCPServer_IP     "192.168.1.105"     //服务器IP "192.168.0.102"
#define User_ESP8266_TCPServer_PORT   "8088"      //服务器端口号



extern volatile uint8_t TcpClosedFlag;  //连接状态标志

void ESP8266_STA_TCPClient_Test(void);

#endif
