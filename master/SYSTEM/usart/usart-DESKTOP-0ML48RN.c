#include "sys.h"
#include "usart.h"	  
#include <stdio.h>
#include "mpu6050.h"  
#include "led.h"
#include "delay.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
	{};//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
	
	
	
	
	typedef union
{	
	signed short intData;
	unsigned char byteData[2];
}INT_BYTE;

static INT_BYTE testpitch,testroll,testyaw;



void USART1_Init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ��
	//GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void 	USART2_Init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //ʹ��USART2��GPIOAʱ��
    
	
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USART2_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3 

  //Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 

}

//void 	USART3_Init(u32 bound)
//{
//  //GPIO�˿�����
//  GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //ʹ��USART3��GPIOBʱ��
//    
//	
//	//USART3_TX   GPIOB.10
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOB.10
//   
//  //USART3_RX	  GPIOB.11��ʼ��
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOB.11 

//  //Usart3 NVIC ����
//  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�2
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//  
//   //USART ��ʼ������

//	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
//  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
//  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3

//}

/*void USART3_IRQHandler(void)                	//����3�жϷ������
{
    
} 
*/
//�ָ��������ã��ָ�����������������ܺ�������ͨ��
 void BTRestore()
{
	extern void Send_datatoU2(int length,u8 *s);
	u8 byteToSend[3]={0x02,0xC0,0x15};
	delay_ms(2);
	Send_datatoU2(3,byteToSend);
	
}
void BTPwrSet()
{
	extern void Send_datatoU2(int length,u8 *s);
	u8 byteToSend[4]={0x03,0xC0,0x12,0x02};
	delay_ms(2);
	Send_datatoU2(4,byteToSend);
}

int utoi(u8 nub)
{
	return (int)nub-48;
}	

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	  
	  extern void Send_datatoU1(int length,u8 *s);
	  extern void Send_datatoU2(int length,u8 *s);
    static struct inf
		{
			u8 IP;
			u8 buwei;
		}message;
	  static u8 suitMAC[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	  u8 head1[4]={0x06,0xC2,0x00,0x00};	
    u8 error0[3]={0xe0,0x55,0x45};
		u8 error1[3]={0xe0,0x88,0x88};	
		u8 error4[3]={0xe4,0x88,0x88};
		u8 er_ck5[3]={0xe5,0x00,0xe5};
		u8 er_ck67[3]={0xe5,0xe6,0xCB};
		u8 er_unknown[3]={0x12,0x34,0x56};
		static u8 check=0x00;
	  u8 Res;
		u8 *R;
		u8 connectgood[3]={0x64,0x00,0x00};
		u8 IDgood[3]={0x61,0x00,0x61};
		u8 bindgood[3]={0x65,0x00,0x65};
		u8 unbindgood[3]={0x66,0x00,0x66};
		u8 NB[3]={0X66,0X66,0X66};
		u8 jiaodu[9]={0x67,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		extern u8 Count;
		u8 m;
		int tint,tint2;

	  float height;
	  static u8 memory[16];
	//	static float pitch,roll,yaw;
		u8 head11[5];
		u8 RR11[3];
	  static int i=0;
		int p=1;
		int j;
	  
	  char str[20];
	  char *s;
	 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
		{
			Count = 0;
			SleepFlag = 0;
			Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
			memory[i]=Res;
			//USART_SendData(USART1,Res);
			if(memory[0]==0x06)					//�������̣���ѯ����δ�󶨴����������ַ������
			{
			 if(i>=6)
				{
					i=0;
					if((memory[1]==0x42)&&(memory[2]==0x00))
					{
						message.IP=memory[6];
						head1[2]=memory[2];
						head1[3]=memory[3];
						connectgood[1]=memory[6];
						connectgood[2]=connectgood[1]+connectgood[0];
												delay_ms(20);
						Send_datatoU2(4,head1);
						Send_datatoU2(3,connectgood);
					}
					else
					{
						head1[2]=memory[2];
						head1[3]=memory[3];
												delay_ms(20);
						Send_datatoU2(4,head1);
						Send_datatoU2(3,error4);
					}
					for(j=0;j<16;j++)
			    {
			     memory[j]=0x00;
			    }
   			  p=0;	
					
			 }
			 if(p==1)
			 {
				i++;
			 }
		 }
			
		 
		 
		 
		 
		 
			if(memory[0]==0x0F)						//����ID���̴���
			{
			if(i>=15)
			{
					i=0;
					for(j=6;j<15;j++)
					{
						check=check+memory[j];
					}
					if(check==memory[15])
					{
							for(j=0;j<10;j++)
							{
								suitMAC[j]=memory[j+6];
							}
						  head1[2]=memory[2];
						  head1[3]=memory[3];
													delay_ms(20);
						Send_datatoU2(4,head1);
						Send_datatoU2(3,IDgood);
					}
					else 
					{
							head1[2]=memory[2];
						  head1[3]=memory[3];
												delay_ms(20);
						  Send_datatoU2(4,head1);
							Send_datatoU2(3,error1);
					}
				  check=0x00;
	  	  for(j=0;j<16;j++)
		    {
			   memory[j]=0x00;
		    }
				p=0;
			 }
			 if(p==1)
			 {
				i++;
			 }			
		}

  }  
		
	
				if(memory[0]==0x09)							//�󶨹��̴���
			  {
					if(i>=9)
				  {
					i=0;
					for(j=6;j<9;j++)
					{
						check=check+memory[j];
					}
					if(check==memory[9])
					{
						head1[2]=memory[2];
						head1[3]=memory[3];
						bindgood[1]=memory[7];
						bindgood[2]=0x65+bindgood[1];
												delay_ms(20);
						Send_datatoU2(4,head1);
						Send_datatoU2(3,bindgood);
						message.buwei=memory[7];
						message.IP=memory[8];
					}
					else 
					{
							head1[2]=memory[2];
						  head1[3]=memory[3];
												delay_ms(20);
						  Send_datatoU2(4,head1);
							Send_datatoU2(3,er_ck5);
					}
				  check=0x00;
	  	  for(j=0;j<16;j++)
		    {
			   memory[j]=0x00;
		    }
				p=0;
			 }
			 if(p==1)
			 {
				i++;
			 }			
		}

		
		
		if(memory[0]==0x08)							//����󶨹��̻�Ƕ����ݴ��䴦��
			  {
					if(i>=8)
				  {
					i=0;
					for(j=6;j<8;j++)
					{
						check=check+memory[j];
					}
					if(check==memory[8])
					{
						if(memory[6]==0x66)										//����󶨹���
						{
							head1[2]=memory[2];
							head1[3]=memory[3];
													delay_ms(20);
							Send_datatoU2(4,head1);
							Send_datatoU2(3,unbindgood);
							message.buwei=0x00;
						}
						if(memory[6]==0x67)										//�Ƕ����ݴ������
						{
							head1[2]=memory[2];
							head1[3]=memory[3];
							delay_ms(2);
							testpitch.intData = (int)(pitch*10);
							testroll.intData = (int)(roll*10);
							testyaw.intData = (int)(yaw*10);
							jiaodu[1]=memory[7];
							jiaodu[2]=testpitch.byteData[1];
							jiaodu[3]=testpitch.byteData[0];
							jiaodu[4]=testroll.byteData[1];
							jiaodu[5]=testroll.byteData[0];
							jiaodu[6]=testyaw.byteData[1];
							jiaodu[7]=testyaw.byteData[0];
							
							/************for test����ʽ�ı�һ�£����㴮�ڵ���������ʾ***********************/							
							/*if(testpitch.intData<0){ 
									jiaodu[2]=0xF0;		//����λ��������λ��F��ʾ������0��ʾ����
									testpitch.intData=(-1)*testpitch.intData;	//ȡ����ֵ
							}else jiaodu[2]=0x00;
							if(testroll.intData<0){
									jiaodu[4]=0xF0;
									testroll.intData=(-1)*testroll.intData;	
							}else jiaodu[4]=0x00;
							if(testyaw.intData<0){
									jiaodu[6]=0xF0;
									testyaw.intData=(-1)*testyaw.intData;
							}else jiaodu[6]=0x00;
							
							//pitch�Ƕȵ�ת��
							tint=testpitch.intData;							
							tint2 = tint/1000;		tint = tint%1000;				jiaodu[2] = jiaodu[2] + tint2;
							tint2 = tint/100;		tint = tint%100;					jiaodu[3]=tint2<<4;  
							tint2 = tint/10;		jiaodu[3]=jiaodu[3]+tint2;  
							//roll�Ƕȵ�ת��
							tint = testroll.intData;
							tint2 = tint/1000;		tint = tint%1000;				jiaodu[4] = jiaodu[4] + tint2;
							tint2 = tint/100;		tint = tint%100;					jiaodu[5]=tint2<<4;  
							tint2 = tint/10;		jiaodu[5]=jiaodu[5]+tint2;  							
							//yaw�Ƕȵ�ת��
							tint = testyaw.intData;
							tint2 = tint/1000;		tint = tint%1000;				jiaodu[6] = jiaodu[6] + tint2;
							tint2 = tint/100;		tint = tint%100;					jiaodu[7]=tint2<<4;  
							tint2 = tint/10;		jiaodu[7]=jiaodu[7]+tint2;  	
							
							/************************************/
							jiaodu[8]=(jiaodu[0]+jiaodu[1]+jiaodu[2]+jiaodu[3]+jiaodu[4]+jiaodu[5]+jiaodu[6]+jiaodu[7]);
							head1[0]=0x0C;
													delay_ms(20);
							Send_datatoU2(4,head1);
							Send_datatoU2(9,jiaodu);
							//delay_ms(50);
							GPIO_SetBits(GPIOB,GPIO_Pin_6);	//LED��							
						}else if(memory[6]==0x68){		//������ģ��ָ���������
							BTRestore();  //���ͻָ�������������
						}else
						{
							head1[2]=memory[2];
							head1[3]=memory[3];
													delay_ms(20);
							Send_datatoU2(4,head1);
							Send_datatoU2(3,er_unknown);
						}
					}
					else 
					{
							head1[2]=memory[2];
						  head1[3]=memory[3];
												delay_ms(20);
						  Send_datatoU2(4,head1);
							Send_datatoU2(3,er_ck67);
					}
				  check=0x00;
					for(j=0;j<16;j++)
					{
						memory[j]=0x00;
					}
					p=0;
			 }
			 if(p==1)
			 {
				i++;
				 GPIO_ResetBits(GPIOB,GPIO_Pin_6);		//LED��							
							
			 }			
		}
  
} 



void USART1_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif

	
	

}


#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif

#endif	