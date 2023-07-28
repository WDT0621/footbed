#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "stmflash.h"
#include <stdio.h>

typedef union
{	
	signed short intData;
	unsigned char byteData[2];
}INT_BYTE;

u8 datatemp[29];
static int Rdflag=0;

int Getflag(void)
{
	if(Rdflag==1)
	{
		return 1;
	}	
	else
	{
		return 0;
	}	
	
}
int main(void)
{
	//u8 Reset[3]={0x02,0xC0,0x15};					//恢复出厂设置
    //u8 del[6] = {0x05,0xC1,0x00,0x32,0x80,0x49};
//	u8 Res;
//	int a=0;
	int i=0;

			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
			USART1_Init(115200);	 	//串口1初始化为115200 USB/蓝牙
			USART2_Init(115200);    //串口2初始化为9600 激光测距模块 10M
		//	USART3_Init(9600);   	//串口3初始化为9600  激光扫平仪模块
			delay_init();	       		//延时初始化 
			LED_Init();		  				//初始化与LED连接的硬件接口
			LED_On();
			//SendToPC(6,del); 				 
			//SendToPC(3,Reset);   //上电对蓝牙复位，测试用
			//BTSendCfg(DEVICE_RESET);
			//BTSendCfg(FACTORY_RESTORE);//恢复出厂设置 for test
			delay_ms(50);			LED_Off();
			
			STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,15);   //第0~7字节是ID,8~14字节是传感器网络地址
			for(i=0;i<8;i++)                                    //读取flash
			{
					suitMAC[i] = datatemp[i];      //ID保存
			}			

			//传感器地址初始化
			if(datatemp[i+8]==0xFF){  
					for(i=0;i<7;i++){
							datatemp[i+8] = 0;
							bindinf[i] = 0;
					}
					STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)datatemp,15);   //将绑定的网络地址初始化为0
			}else{
					for(i=0;i<7;i++) bindinf[i]=datatemp[i+8];				
			}
			
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口1接受中断
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口2接受中断
			
			delay_ms(50);LED_On();
			delay_ms(50);LED_Off();
			delay_ms(10);	LED_On();
			delay_ms(10);LED_Off();
			delay_ms(10);	LED_On();
						
			while(1)
			{
					 FrameParse1();					//USART1通信包解析--上位机通信
				   FrameParse2();					//USART2通信包解析--蓝牙模块通信 zhh bug --没调用
			}					

			 	
}
 


