#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "wkup.h"


u8 StopFlag = 0;
extern u8 SleepFlag;
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
void EXTIX_Init(void)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
    KEY_Init();	 //	按键端口初始化
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	  //GPIOA.15-->BT_RES	  中断线以及中断初始化配置  下降沿触发	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	  EXTI_InitStructure.EXTI_Line=EXTI_Line15;	
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	  //GPIOB.10-->KEY_ON	  中断线以及中断初始化配置 下降沿触发   
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10); 
  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

		//使能按键BT_RES所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
		//使能按键KEY_ON所在的外部中断通道
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void EXTI15_10_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line15)!=RESET )
		{
				delay_ms(10);//消抖
				if(BT_RES==0)
				{
						LED1 = 1;					//两灯灭,LED2亮灭
						LED2 = 1;
						BTRestore();
						LED2 = 0;
						delay_ms(1000);
						LED2 = 1;
				}		 
				EXTI_ClearITPendingBit(EXTI_Line15);  //清除LINE15上的中断标志位  
		}
		if(EXTI_GetITStatus(EXTI_Line10)!=RESET )
		{
				delay_ms(10);
				if(Check_WKUP() == 0)//关机？
				{		  
						Count = 0;
						if(SleepFlag == 1)
						{
								SleepFlag = 0;
								TIM_Cmd(TIM3, ENABLE);
								LED1 = 0;delay_ms(80);
								LED1 = 1;delay_ms(200); 
								LED1 = 0;delay_ms(300); 
								LED1 = 1;delay_ms(200); 
								LED1 = 0;delay_ms(1200);
								LED1 = 1;
						}
						else
						{
								StopFlag = !StopFlag;
								if(StopFlag == 1)
								{
										LED1 = 0;delay_ms(500);
										LED1 = 1;delay_ms(200); 
										LED1 = 0;delay_ms(200); 
										LED1 = 1;delay_ms(200); 
										LED1 = 0;delay_ms(80);
										LED1 = 1;
							  }		
								else
								{
										SYSCLKConfig_STOP();
										delay_ms(80);
										LED1 = 0;delay_ms(80);
										LED1 = 1;delay_ms(200); 
										LED1 = 0;delay_ms(300); 
										LED1 = 1;delay_ms(200); 
										LED1 = 0;delay_ms(800);
										LED1 = 1;
								}
						}

				}
				else
				{
						LED1 = 0;delay_ms(9000);
						PWR_EN = 0;
						LED1 = 1;delay_ms(5000);		//关机		
				}
		}		 
		EXTI_ClearITPendingBit(EXTI_Line10);  //清除LINE10上的中断标志位  
}
	



/*

//外部中断15服务程序 
void EXTI15_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(BT_RES==0)	 	 //BT_RES按键
	{				 
	
		LED1 = !LED1;
	
	}
	EXTI_ClearITPendingBit(EXTI_Line15); //清除LINE15上的中断标志位  
}
 
//外部中断11服务程序
void EXTI11_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY_ON==1)	 //按键KEY_ON
	{
		//LED0=!LED0;
		delay_ms(1000);
		if(KEY_ON==0)
		{
      GPIO_ResetBits(GPIOB,GPIO_Pin_10);//休眠
		}
		else
		{
			PWR_EN = 0;
		}
	}		 
	EXTI_ClearITPendingBit(EXTI_Line11);  //清除LINE11上的中断标志位  
}
 */
