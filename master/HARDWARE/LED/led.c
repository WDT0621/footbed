#include "led.h"
    
//LED IO��ʼ��
void LED_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA��PB�˿�ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //����AFIO����ʱ��
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//��PB3��PB4��PA15����ͨIO��PA13&14����SWD������ӳ��
	
		/*===============================================LED=================================================*/
		//LED1-->PB.6 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					     
		//LED2-->PB.7 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	           
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		/*===============================================PWR_EN===============================================*/
		//PWR_EN-->PA.0 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	           
		GPIO_Init(GPIOA, &GPIO_InitStructure);			
	
		/*======================================CHRG(�°�ȡ����)================================================*/
		//CHRG-->PA.1 �˿�����
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				     
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		   	//��������
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO���ٶ�Ϊ50MHz
//		GPIO_Init(GPIOA, &GPIO_InitStructure);		
		
		/*=============================================CHRG_WKUP================================================*/
		//CHRG_WKUP-->PB.11 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
		/*=========================================������������GPIO�˿�=======================================*/
		//�������е�1��-->PB.5 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//�������е�2��-->PB.12 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//�������е�3��-->PB.4 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					      
		//�������е�4��-->PB.13 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//�������е�5��-->PB.3 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					      
		//�������е�6��-->PB.14 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//�������е�7��-->PA.12 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					      
		//�������е�8��-->PB.15 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);							
		//�������е�9��-->PA.11 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				    
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					       
		//�������е�10��-->PA.8 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					       
		//�������е�11��-->PA.10 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					       
		//�������е�12��-->PA.9 �˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					      

		GPIO_SetBits(GPIOB,GPIO_Pin_6);						           //PB.6 ����� LED1��
		GPIO_SetBits(GPIOB,GPIO_Pin_7);						           //PB.7 ����� LED2��
		GPIO_ResetBits(GPIOA,GPIO_Pin_0);						         //PA.0 ����� PWR��ʹ��
		//��ʼ���������������
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_4);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_3);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_11);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_9);
}
 
