#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f4xx.h"
#include "sys.h"


/*
*���²�����Ҫ�û������޸Ĳ��ܲ����ù�
*/
#define User_ESP8266_SSID     "TPlink-733"          //wifi��
#define User_ESP8266_PWD      "dianzixi733"      //wifi����

#define User_ESP8266_TCPServer_IP     "192.168.1.105"     //������IP "192.168.0.102"
#define User_ESP8266_TCPServer_PORT   "8088"      //�������˿ں�



extern volatile uint8_t TcpClosedFlag;  //����״̬��־

void ESP8266_STA_TCPClient_Test(void);

#endif
