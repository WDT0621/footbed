#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "wkup.h"


u8 StopFlag = 0;
extern u8 SleepFlag;
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
    KEY_Init();	 //	�����˿ڳ�ʼ��
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

	  //GPIOA.15-->BT_RES	  �ж����Լ��жϳ�ʼ������  �½��ش���	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	  EXTI_InitStructure.EXTI_Line=EXTI_Line15;	
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	  //GPIOB.10-->KEY_ON	  �ж����Լ��жϳ�ʼ������ �½��ش���   
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10); 
  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

		//ʹ�ܰ���BT_RES���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
		//ʹ�ܰ���KEY_ON���ڵ��ⲿ�ж�ͨ��
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

void EXTI15_10_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line15)!=RESET )
		{
				delay_ms(10);//����
				if(BT_RES==0)
				{
						LED1 = 1;					//������,LED2����
						LED2 = 1;
						BTRestore();
						LED2 = 0;
						delay_ms(1000);
						LED2 = 1;
				}		 
				EXTI_ClearITPendingBit(EXTI_Line15);  //���LINE15�ϵ��жϱ�־λ  
		}
		if(EXTI_GetITStatus(EXTI_Line10)!=RESET )
		{
				delay_ms(10);
				if(Check_WKUP() == 0)//�ػ���
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
						LED1 = 1;delay_ms(5000);		//�ػ�		
				}
		}		 
		EXTI_ClearITPendingBit(EXTI_Line10);  //���LINE10�ϵ��жϱ�־λ  
}
	



/*

//�ⲿ�ж�15������� 
void EXTI15_IRQHandler(void)
{
	delay_ms(10);//����
	if(BT_RES==0)	 	 //BT_RES����
	{				 
	
		LED1 = !LED1;
	
	}
	EXTI_ClearITPendingBit(EXTI_Line15); //���LINE15�ϵ��жϱ�־λ  
}
 
//�ⲿ�ж�11�������
void EXTI11_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY_ON==1)	 //����KEY_ON
	{
		//LED0=!LED0;
		delay_ms(1000);
		if(KEY_ON==0)
		{
      GPIO_ResetBits(GPIOB,GPIO_Pin_10);//����
		}
		else
		{
			PWR_EN = 0;
		}
	}		 
	EXTI_ClearITPendingBit(EXTI_Line11);  //���LINE11�ϵ��жϱ�־λ  
}
 */
