#include "sys.h"
#include "usart2.h"	 
#include "esp8266.h"
#include "tcp.h"

void usart2_init(u32 pclk1,u32 bound)
{  	 
  
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟

	//USART1_TX   PA.2 PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2,PA3,复用功能,上拉输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2，PA3
	
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
  
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;	// 发模式
    
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    USART_Init(USART2, &USART_InitStructure); //初始化串口
    
    USART_ITConfig(USART2,USART_IT_RXNE|USART_IT_IDLE,ENABLE);
    
    USART_Cmd(USART2, ENABLE);  //使能串口 
	
}

extern volatile uint8_t TcpClosedFlag;  //连接状态标志  tcp.c里定义
void USART2_IRQHandler(void)
{   
    u8 ucCh;

    if(USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
    {
        ucCh  = USART_ReceiveData( USART2 );
        USART_SendData(USART1,ucCh);
        if(ESP8266_Fram_Record_Struct .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 
        {

            ESP8266_Fram_Record_Struct .Data_RX_BUF[ ESP8266_Fram_Record_Struct .InfBit .FramLength ++ ]  = ucCh;   
        }                      
        
    }

    if( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //如果总线空闲
    {
        ESP8266_Fram_Record_Struct .InfBit .FramFinishFlag = 1;

        ucCh = USART_ReceiveData( USART2 );                                                              //由软件序列清除中断标志位（先读USART_SR,然后读USART_DR）
		USART_SendData(USART1,ucCh);
        TcpClosedFlag = strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;

    }   

}






