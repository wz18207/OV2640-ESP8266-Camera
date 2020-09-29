#include "tcp.h"
#include "usart.h"
#include "esp8266.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx.h"
#include "sys.h"

volatile u8 TcpClosedFlag = 0;

void ESP8266_STA_TCPClient_Test(void)
{
    u8 res;
	
    char str[100]={0};
    ESP8266_AT_Test();
		printf("��������ESP8266\r\n");
    ESP8266_Net_Mode_Choose(STA);
    while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
    ESP8266_Enable_MultipleId ( DISABLE );
    while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));
    while(!ESP8266_UnvarnishSend());
	printf("\r\n�������");
    while ( 1 )
    {       
			  sprintf (str,"�����а��ſɿƼ����޹�˾" );//��ʽ�������ַ�����TCP������
        ESP8266_SendString ( ENABLE, str, 0, Single_ID_0 );
        delay_ms(1000);
        if(TcpClosedFlag) //�ж��Ƿ�ʧȥ����
        {
            ESP8266_ExitUnvarnishSend(); //�˳�͸��ģʽ
            do
            {
                res = ESP8266_Get_LinkStatus();     //��ȡ����״̬
            }   
            while(!res);

            if(res == 4)                     //ȷ��ʧȥ���ӣ�����
            {
                
                
                while (!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD ) );
                while (!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0 ) );        
            } 
            while(!ESP8266_UnvarnishSend());                    
        }
    }   
}
