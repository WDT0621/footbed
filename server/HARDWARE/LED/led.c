#include "led.h"
    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				     //LED1-->PC.13 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					       //�����趨������ʼ��GPIOC.13
 GPIO_SetBits(GPIOC,GPIO_Pin_13);						           //PC.13 ����� LED��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				     //LED2-->PC.14 �˿�����
 GPIO_Init(GPIOC, &GPIO_InitStructure);					       //�����趨������ʼ��GPIOC.14
 GPIO_SetBits(GPIOC,GPIO_Pin_14);						           //PC.14 ����� LED��
}
 
void LED_On(void){
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//PC.13 ����͵�ƽ ����LED
}
void LED_Off(void){
		GPIO_SetBits(GPIOC,GPIO_Pin_13); 		//PC.13 ����ߵ�ƽ LED��
}

	
