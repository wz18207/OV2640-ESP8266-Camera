#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"  
#include "usart2.h"  
#include "timer.h" 
#include "ov2640.h" 
#include "dcmi.h" 
#include "esp8266.h"
#include "tcp.h"


u8 ov2640_mode=0;						//工作模式:0,RGB565模式;1,JPEG模式

#define jpeg_buf_size 31*1024  			//定义JPEG数据缓存jpeg_buf的大小(*4字节)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG数据缓存buf
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
//JPEG尺寸支持列表
const u16 jpeg_img_size_tbl[][2]=
{
	176,144,	//QCIF
	160,120,	//QQVGA
	352,288,	//CIF
	320,240,	//QVGA
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,1024,	//SXGA
	1600,1200,	//UXGA
}; 
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
const u8*JPEG_SIZE_TBL[9]={"QCIF","QQVGA","CIF","QVGA","VGA","SVGA","XGA","SXGA","UXGA"};	//JPEG图片 9种尺寸 


//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	if(ov2640_mode)//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//停止当前传输 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//得到此次数据传输的长度
				
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//传输长度为jpeg_buf_size*4字节
			DMA_Cmd(DMA2_Stream1, ENABLE);			//重新传输
			jpeg_data_ok=0;						//标记数据未采集
		}
	}
} 
//JPEG测试
//JPEG数据,通过串口2发送给电脑.
void jpeg_test(void)
{
	u32 i; 
	u8 *p;
	u8 key;
	u8 effect=0,saturation=2,contrast=2;
	u8 size=3;		//默认是QVGA 320*240尺寸
	u8 msgbuf[15];	//消息缓存区 

	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	
 	OV2640_JPEG_Mode();		//JPEG模式
	My_DCMI_Init();			//DCMI配置
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA配置   
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸 
	DCMI_Start(); 		//启动传输
	//delay_ms(500);
	while(1)
	{
		if(jpeg_data_ok==1)	//已经采集完一帧图像了
		{ 
			//for(i = 0;i<jpeg_buf_size;i=i+4)
			//jpeg_buf[i] = ((jpeg_buf[i]>>11)*19595+((jpeg_buf[i]>>5)&0x3f)*38469+(jpeg_buf[i]&0x1f)*7472)>>16;
			
	//		p=(u8*)jpeg_buf;
			
			p=(u8*)jpeg_buf;
			//printf("Sending JPEG data...");
		//	LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //提示正在传输数据
			for(i=0;i<jpeg_data_len*4;i++)		//dma传输1次等于4字节,所以乘以4.
			{
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);	//循环发送,直到发送完毕  
    				
				
				USART_SendData(USART2,p[i]); 
				
				//delay_ms(10);
			}
			delay_ms(10);
			//printf("%d",jpeg_data_ok);
//			printf("\r\nSend data complete!\r\n");// LCD_ShowString(30,210,210,16,16,"Send data complete!!");//提示传输结束设置 
			jpeg_data_ok=2;	//标记jpeg数据处理完了,可以让DMA去采集下一帧了.
		}		
		
		
	}    
} 

int main(void)
{ 
	u8 key;
	u8 t;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart2_init(42,115200);		//初始化串口2波特率为115200

    ESP8266_Init();
	TIM3_Int_Init(10000-1,8400-1);//10Khz计数,1秒钟中断一次

	while(OV2640_Init())//初始化OV2640
	{
	//	LCD_ShowString(30,130,240,16,16,"OV2640 ERR");
		delay_ms(200);	
		printf("error");
	}
	//LCD_ShowString(30,130,200,16,16,"OV2640 OK");  
    
		printf("正在配置ESP8266\r\n"); 
    ESP8266_AT_Test();
		
    ESP8266_Net_Mode_Choose(STA);
    while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
    ESP8266_Enable_MultipleId ( DISABLE );
    while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));
    while(!ESP8266_Send_AT_Cmd("AT+UART_CUR=921600,8,1,0,0","OK",NULL,500));
    usart2_init(42,921600);		//初始化串口2波特率为921600
    while(!ESP8266_UnvarnishSend());  //开启透传模式
		printf("\r\n esp8266配置完成\r\n");    
//    
// 	while(1)
//	{	
//		key=KEY_Scan(0);
//		if(key==KEY0_PRES)			//RGB565模式
//		{
//			ov2640_mode=0;   
//			break;
//		}else if(key==KEY1_PRES)	//JPEG模式
//		{
//			ov2640_mode=1;
//			break;
//		}
//		t++; 									  
//		if(t==100)LCD_ShowString(30,150,230,16,16,"KEY0:RGB565  KEY1:JPEG"); //闪烁显示提示信息
// 		if(t==200)
//		{	
//			LCD_Fill(30,150,210,150+16,WHITE);
//			t=0; 
//		}
//		delay_ms(5);	  
//	}

		ov2640_mode=1;
		jpeg_test();
 
}
