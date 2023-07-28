#include "sys.h"
#include "usart.h"	  
#include <stdio.h>
#include "mpu6050.h"  
#include "led.h"
#include "delay.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "stmflash.h"
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

u8 bindinf[7];		//��������״̬
u8 pCurrent;  		//��ǰλ��

//����1����
u8 commStatus1 = COMM_IDLE; 			//����1ͨ��״̬
u8 commRcvBuff1[BUFF_LEN_MAX1];  	//����1���ջ�����
u8 commSendBuff1[BUFF_LEN_MAX1];  //����1���ͻ�����
u8 frameLen1;           					//����1֡��

//����2����
u8 commStatus2 = COMM_IDLE;				//����2ͨ��״̬
u8 commRcvBuff2[BUFF_LEN_MAX2];  	//����2���ջ�����---��󳤶�20
u8 commSendBuff2[BUFF_LEN_MAX2];  //����2���ͻ�����
u8 frameLen2;           					//����2֡��

//��������
u8 msgCtlType;			//������Ϣ����
u8 msgOpCode;				//������
u8 msgRcvType;			//�յ�����Ϣ����
u16 addrSource;			//ԭ��ַ
u16 addrTarget;			//Ŀ���ַ
u8 btMode = 0;				//����ͨ��ģʽ��0��ʾһ��ģʽ��1��ʾ͸��ģʽ
u8 netAddStatus = NETADD_IDLE;  //��������״̬
u8 btErrCode = 0;		//����������
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

 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ

static u8 SaveBuffer[15];//����Flash��ȡ
u8 suitMAC[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

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

//�������ݵ���λ��--ͨ������1
void SendToPC(u8 length ,u8 *s)
{
	int i=0;
	    delay_ms(2);//QQQΪʲô��Ҫ��ʱ��---��δŪ��ԭ��
      for(i=0;i<length;i++)
      {
					while((USART1->SR&0x0080)==0); //�����ݻ�û�б�ת�Ƶ���λ�Ĵ�����ȴ�
					USART1->DR=s[i];//����һ���ֽڵ�����
      }
}

//�������ݵ�����ģ�飨�����봫����ͨ�ţ�-ͨ������2
void BTSendData(int length ,u8 *s)
{
	int i=0;
			delay_ms(2);	//������Ϊʲô��Ҫ��ʱ��
      for(i=0;i<length;i++)
      {
					while((USART2->SR&0x0080)==0); //�����ݻ�û�б�ת�Ƶ���λ�Ĵ�����ȴ�
					USART2->DR=s[i];//����һ���ֽڵ�����
      }
}
//������������--�������ֻ������3�ֽڶ�ָ��
//���룺inOpCode��������
void BTSendCfg(u8 inOpCode)
{
		commSendBuff2[0]=2;
	  commSendBuff2[1]=CONTROL_MSG_CONFIG;
	  commSendBuff2[2]=inOpCode;
		BTSendData(3,commSendBuff2);
}
//������������--������ַ��������
//���룺inAddr--��һ������ʱ�Ľ�Ҫ������������ĵ�ַ
void BTSetNextAddr(u16 inAddr)
{
		commSendBuff2[0]=4;
	  commSendBuff2[1]=CONTROL_MSG_CONFIG;
	  commSendBuff2[2]=NEXT_ADDR_SET;		//������
	  commSendBuff2[3]=inAddr/256;
	  commSendBuff2[4]=inAddr%256;	
		BTSendData(5,commSendBuff2);
}
//������������--SIG���ɾ���ڵ�����---�����֧�֣���δʹ��
//���룺inNodeAddr���ڵ������ַ
void BTDelNode(u16 InNodeAddr)
{
		commSendBuff2[0]=5;
	  commSendBuff2[1]=CONTROL_MSG_SIG;  //��SIG����ɾ���ڵ�
	  commSendBuff2[2]=InNodeAddr/256;
	  commSendBuff2[3]=InNodeAddr%256;
	  commSendBuff2[4]=0x80;
	  commSendBuff2[5]=0x49;
		BTSendData(6,commSendBuff2);
}
//�����������������--ͨ��͸����Ϣ���ע������ģ���͸������--����λ����͸����ͬ��
//���룺inCmdCode�������룻inPosition��������λ�ã�inNodeAddr�������������ַ
void BTCmdToSensor(u8 inCmdCode,u8 inPosition,u16 InNodeAddr)
{
		commSendBuff2[0]=6;
	  commSendBuff2[1]=CONTROL_MSG_TRANS;  
	  commSendBuff2[2]=InNodeAddr/256;
	  commSendBuff2[3]=InNodeAddr%256;
	  commSendBuff2[4]=inCmdCode;
	  commSendBuff2[5]=inPosition;
	  commSendBuff2[6]=commSendBuff2[4] + commSendBuff2[5];//У���
		BTSendData(7,commSendBuff2);
}
//У��ͼ���
//���룺inLen��֡���ȣ�inBuff[]��������
//     ע�⣺inLen����У����ֽڵĳ���
//					���Ǽ���ʱ�����������һ��У����ֽ�
u8 GetCheckSum(u8 inLen,u8 *inBuff)
{
	u8 i;
	u8 tcheckSum = 0;
	
		for(i=0;i<inLen-1;i++)
		{
			tcheckSum = tcheckSum+inBuff[i];
		}
		return tcheckSum;
}

//У����ж�
//���룺inLen��֡���ȣ�buff[]��������
//�����True��У����ȷ��False��У�����
u8 CheckSumTest(u8 inLen,u8 *inBuff)
{		
		return GetCheckSum(inLen,inBuff)==inBuff[inLen-1];
}
//���Ͷ����3�ֽڣ�����λ����������+����+У���
//���룺inCmdCode�������룻inParam��1�ֽڲ���
void SendShortCmd(u8 inCmdCode,u8 inParam){
u8 tBuff[3];
	
	  tBuff[0] = inCmdCode; //���λ��1����Ϊ���صĴ���������
	  tBuff[1] = inParam;
	  tBuff[2] = GetCheckSum(3,tBuff);
    SendToPC(3,tBuff);
}


//�����յ���ͨ��֡
void FrameParse1(void){

		u8 j;  					//�ֲ�ѭ������
		u8 cmdCode;			//������
		u8 cmdCodeErr;	//�������Ӧ�Ĵ���������
		
    if(commStatus1 == COMM_FRAME_READY){	
				cmdCode = commRcvBuff1[0];  
				cmdCodeErr = cmdCode|0x80;
				if(!CheckSumTest(frameLen1,commRcvBuff1))//У����ж�
				{
						SendShortCmd(cmdCodeErr,COMM_CHECKSUM_ERR);					
				}else{
						switch(cmdCode){
						case 	COMM_SHAKEHAND:		//PC��������ָ��--0x60 --   to test in Hex��60 55 B5
									if(commRcvBuff1[1]!=0x55){
											SendShortCmd(cmdCodeErr,COMM_SHAKEHAND_ERR);
									}else{							
											SendShortCmd(cmdCode,0xAA);
									}
						break;				
						case COMM_SET_ID:				//�����豸ID -0x61-- to test in Hex��61 20 21 01 26 00 01 01 00 CB
									for(j=0;j<8;j++)
									{
										suitMAC[j]=commRcvBuff1[j+1];        //2~8�ֽ�Ϊ�豸ID������suitMAC
										SaveBuffer[j] = suitMAC[j];//����Ҫ�����ݴ��뻺�����
									}
									STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);
									STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);          //�ٶ������м���
									for(j=0;j<8;j++)                                  
									{
											if(suitMAC[j] != SaveBuffer[j])break;
									}
									if (j<8){
											SendShortCmd(cmdCodeErr,COMM_FLASH_ERR);
									}else{
											SendShortCmd(cmdCode,COMM_NO_ERR);
									}
						break;
						case COMM_READ_ID:				//��ȡ�豸ID-0x62--to test in Hex��62 00 62
									STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);    		//0~7λ��ID,8~14λ�ǰ󶨵�ַ
									for(j=0;j<8;j++) suitMAC[j] = SaveBuffer[j];									
									commSendBuff1[0] = COMM_READ_ID;
									for(j=0;j<8;j++) commSendBuff1[j+1]=suitMAC[j];
									commSendBuff1[9] = GetCheckSum(10,commSendBuff1);			//����У���
									SendToPC(10,commSendBuff1);				//����IDֵ
						break;	
						case COMM_SENSOR_STATUS:				//��ѯ��״̬-0x63--to test in Hex��63 00 63
									STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);          //0~7λ��ID,8~14λ�ǰ󶨵�ַ
									for(j=0;j<7;j++) bindinf[j] = SaveBuffer[j+8];																		
									commSendBuff1[0]=0X63;
									commSendBuff1[1]=0X07;
									for(j=0;j<7;j++) commSendBuff1[j+2]=bindinf[j];
									commSendBuff1[9] = GetCheckSum(10,commSendBuff1);//����У���
									SendToPC(10,commSendBuff1);				//���Ͱ�״ֵ̬
						break;				
						case COMM_BIND_BEGIN:		//�󶨴�����   -0x64--to test in Hex��64 01 65  (��λ��1�Ĵ�����)
									if(commRcvBuff1[1]==0||commRcvBuff1[1]>7){		//���Ĵ�����λ���ж�
											SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);				
									}else{
											STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);          //0~7λ��ID,8~14λ�ǰ󶨵�ַ
											for(j=0;j<7;j++)	bindinf[j] = SaveBuffer[j+8];								
											j = commRcvBuff1[1]-1;		//ͨ���������commRcvBuff1[1]Ϊ������λ��
											if(bindinf[j]==0){      	//�ж��Ƿ��Ѿ����д�����
													SendShortCmd(cmdCode,0);        
													BTSendCfg(NETWORK_ADDIN);				//��������ָ��
													pCurrent = commRcvBuff1[1];     //�����λ��
												  netAddStatus = NET_ADDING;			//��������
											}else{    //����Ѿ��󶨴����������ش���
													SendShortCmd(cmdCodeErr,COMM_OTHER_BOUND_ERR);			
											}
									}
						break;
						case COMM_BIND_CHECK:							//0x65��ѯ�󶨽���
									if(netAddStatus==NETADD_IDLE){   					//�ް󶨹���
											SendShortCmd(cmdCodeErr,BT_NET_ADDING_NULL);								
							    }else if(netAddStatus==NET_ADDING){   		//��δ���
											SendShortCmd(cmdCodeErr,BT_NET_ADDING_ONGO);
									}else if(netAddStatus==NETADD_FLASH_FAILURE){   		//����FLASHʱ��������
											SendShortCmd(cmdCodeErr,BT_NET_ADDING_FLASH_ERR);
									}else	if(netAddStatus==NETADD_SUCCESS){	//�󶨳ɹ�
											SendShortCmd(cmdCode,bindinf[pCurrent-1]);	//�ش�PC�����ز���Ϊ�����������ַ���յ���Ϣʱbindinf[]�Ѹ��£�
											netAddStatus = NETADD_IDLE;
									}else{																		//��������
										  SendShortCmd(cmdCodeErr,btErrCode); 			//��������������
											netAddStatus = NETADD_IDLE;
									}									
						break;	
						case COMM_UNBIND_SENSOR:	//��󴫸���  -0x66--to test in Hex��66 01 67  (���λ��1�Ĵ�����)
									if(commRcvBuff1[1]==0||commRcvBuff1[1]>7){		
											SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);				//���Ĵ�����λ�ô���
									}else{																			
											STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15); 
											for(j=0;j<7;j++)	bindinf[j] = SaveBuffer[j+8];								
											pCurrent = commRcvBuff1[1];							//�󶨵�λ��
											if(bindinf[pCurrent-1]!=0){   					//������д�����--�ɽ��
														//���������������������ģ��ָ���������
												  BTCmdToSensor(COMM_BT_RESTORE,commRcvBuff1[1],(u16)bindinf[pCurrent-1]); 
														//�޸ı��浽FLASH
													bindinf[pCurrent-1] = 0;    				//��Ϊ0����ʾû�󶨴�����
													SaveBuffer[pCurrent+7] = 0;	
													STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);	
														//�ش�PC����֪���ɹ�
													SendShortCmd(COMM_UNBIND_SENSOR,pCurrent);																					
											}else{ 									
													SendShortCmd(cmdCodeErr,COMM_NO_BOUND_ERR);	//û�а��д�����--����
											}
									}
						break;
						case COMM_BT_RESTORE:			//0x68		����ģ��ָ���������									
								 BTSendCfg(FACTORY_RESTORE);
						break;		
						case COMM_READ_PRESSURE:			//0x69		��ȡѹ��									
								if(commRcvBuff1[1]==0||commRcvBuff1[1]>0x07)
								{
										SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);										
								}else{
										j = commRcvBuff1[1]-1;		//commRcvBuff1[1]Ϊ������λ��
										if(bindinf[j]!=0){      	//������д�����
												
												commSendBuff2[0]=7;
												commSendBuff2[1]=CONTROL_MSG_TRANS;  
												commSendBuff2[2]=(u16)bindinf[j]/256;
												commSendBuff2[3]=(u16)bindinf[j]%256;
												commSendBuff2[4]=0x69;
												commSendBuff2[5]=commRcvBuff1[1];
												commSendBuff2[6]=commRcvBuff1[2];
												commSendBuff2[7]=commSendBuff2[6] + commSendBuff2[5] + commSendBuff2[4];//У���
												BTSendData(8,commSendBuff2);
										}else{ 									
												SendShortCmd(cmdCodeErr,COMM_NO_BOUND_ERR);		 //��λ��û�а󶨴�����
										}
								}								 
						break;						
						case COMM_BT_TRANSPORT:		//��λ����ʹ�õ�����͸��ָ��					
								 btMode = 1;
								 BTSendData(commRcvBuff1[1]+1,commRcvBuff1+1);//��2�ֽ�+1Ϊ֡�����ӵ�2�ֽڿ�ʼ����
						break;
						case COMM_READ_ANGLE:		//���������ĽǶ�����
							  //QQQ��ʱ��Ҫ����
								if(commRcvBuff1[1]==0||commRcvBuff1[1]>0x07)
								{
										SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);										
								}else{
										j = commRcvBuff1[1]-1;		//commRcvBuff1[1]Ϊ������λ��
										if(bindinf[j]!=0){      	//������д�����
												BTCmdToSensor(cmdCode,commRcvBuff1[1],(u16)bindinf[j]);  //����͸�������
										}else{ 									
												SendShortCmd(cmdCodeErr,COMM_NO_BOUND_ERR);		 //��λ��û�а󶨴�����
										}
								}
						default:								
						break;
						}
				}
				commStatus1 = COMM_IDLE;	//������ɣ�ͨ��״̬��Ϊ����
				LED_Off();	delay_ms(5);	LED_On();//LED��˸��ָʾ�յ�����
		}			
}
//������ģ�������Ϣ�Ĵ���
//���룺������ inOpCode������ inData
void BTDealCfg(u8 inOpCode,u8 * inData)
{
	u16 i;
		switch(inOpCode){
			case  NETKEY_SET:		  			//���������ܳף�ֻ������ģ�飩0x00	
			break;
			case  NETKEY_RD:		  			//�������ܳף�����0x01	
			break;
			case  APPKEY_SET:		  			//����Ӧ���ܳף�����0x02
			break;
			case  APPKEY_RD:		  			//��Ӧ���ܳף�����0x03	
			break;
			case  NEXT_ADDR_SET:  			//�����´ε�������ַ������0x04	
			break;
			case  NEXT_ADDR_RD:	  			//���´ε�������ַ������0x05
			break;			
			case  NETWORK_ADDIN: 				//�豸����������0x09	
			      STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);    
						for(i=0;i<7;i++)	bindinf[i] = SaveBuffer[i+8];												
						i = pCurrent - 1;    
						bindinf[i] = inData[7];    				//�����������ַ
						SaveBuffer[i+8] = bindinf[i];	
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);//����FLASH��			
						STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15); //�ٶ����������Ƿ񱣴���ȷ
						for(i=0;i<7;i++){	
								if(bindinf[i] != SaveBuffer[i+8])	break;			
						}
						if(i>=7)
								netAddStatus = NETADD_SUCCESS;			//�����ɹ����ȴ���ѯ			
						else
								netAddStatus = NETADD_FLASH_FAILURE;
			break;			
			default:
					//QQQ�������
			break;
		}
}	

//�������2������ģ�飩�յ���ͨ��֡
void FrameParse2(void){
u16 i;
u16 tFrameLen;
	
    if(commStatus2 == COMM_FRAME_READY){	
				if(btMode==1){	//�յ����������������͵���λ��
					  commSendBuff1[0]=COMM_BT_TRANSPORT;
					  tFrameLen = commRcvBuff2[0]+1;		//zhh bug commSendBuff2->commRcvBuff2
					  for(i=0;i<tFrameLen;i++){
								commSendBuff1[i+1]=commRcvBuff2[i];
						}
						tFrameLen = tFrameLen + 2;
						commSendBuff1[tFrameLen-1] = GetCheckSum(tFrameLen,commSendBuff1);//����У���
						SendToPC(tFrameLen,commSendBuff1);  //���͵���λ��  zhh bug commSendBuff2 ->commSendBuff1
						btMode = 0;
				}else{			//����״̬��������Ϣ����
						msgCtlType = commRcvBuff2[1];
						switch(msgCtlType){					
							case RESPONSE_MSG_CONFIG:	//���ò�����Ϣ  -0x40-				
									BTDealCfg(commRcvBuff2[2],commRcvBuff2 + 3);	//���������Ϣ
							break;
							case RESPONSE_MSG_SIG:		//SIG��Ϣ -0x41----Ŀǰֻ����ɾ���ڵ���Ϣ---�����ԣ������֧��!
									/*if(commRcvBuff2[0]==7&&commRcvBuff2[6]==0x80 && commRcvBuff2[7]==0x4A){//ɾ���ڵ�ɹ����յ��ĳ���Ϊ7�������2�ֽ�������ȷ									
											STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);    
											for(i=0;i<7;i++)	bindinf[i] = SaveBuffer[i+8];
											if(pCurrent>0 && pCurrent<=0x07 && bindinf[pCurrent-1]!=0){
													bindinf[pCurrent-1] = 0;    	//��Ϊ0����ʾû�󶨴�����
													SaveBuffer[pCurrent+7] = 0;	
													STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);		//����FLASH��												  
													SendShortCmd(COMM_UNBIND_SENSOR,pCurrent);							//�ش�PC�����ɹ�
											}
									}	*/								
							break;
							case RESPONSE_MSG_TRANS:	//͸����Ϣ -0x42--Ŀǰֻ������ǶȺ�ѹ����������Ϣ
										if(commRcvBuff2[0]==14 && commRcvBuff2[6]==COMM_READ_ANGLE)
										{										  
											if(!CheckSumTest(9,commRcvBuff2+6))
											{//У����ж�											
													SendShortCmd(COMM_READ_ANGLE|0x80,COMM_CHECKSUM_ERR);	
											}
											else
											{
													SendToPC(9,commRcvBuff2+6);	//�ش��Ƕ����ݸ���λ����֡��ʽ�ڴ��������Ѵ��
											}	
										}
										if(commRcvBuff2[0]==33 && commRcvBuff2[6]==COMM_READ_PRESSURE)
										{										  
											if(!CheckSumTest(28,commRcvBuff2+6))
											{//У����ж�											
													SendShortCmd(COMM_READ_PRESSURE|0x80,COMM_CHECKSUM_ERR);	
											}
											else
											{
													SendToPC(28,commRcvBuff2+6);	//�ش�ѹ�����ݸ���λ����֡��ʽ�ڴ��������Ѵ��
											}	
										}
//										if(commRcvBuff2[0]==57 && commRcvBuff2[6]==COMM_READ_PRESSURE)
//										{										  
//											if(!CheckSumTest(52,commRcvBuff2+6))
//											{//У����ж�											
//													SendShortCmd(COMM_READ_PRESSURE|0x80,COMM_CHECKSUM_ERR);	
//											}
//											else
//											{
//													SendToPC(52,commRcvBuff2+6);	//�ش�ѹ�����ݸ���λ����֡��ʽ�ڴ��������Ѵ��
//											}	
//										}
							break;
							case RESPONSE_MSG_ERR:	//��������-0x43-  here Ҳ�����зǴ�����Ϣ���ָ�����������								  
									btErrCode = commRcvBuff2[3];    //��������������
							    if(commRcvBuff2[2]==NETWORK_ADDIN){//����ǰ󶨴��������̣����������̣�
										   netAddStatus = NETADD_FAILURE;
									}
									if(commRcvBuff2[2]==DEVICE_RESET){ //��������ģ��(��&��)0x14									
											if(btErrCode==0){
													//here ��������ģ��ɹ�
											}
									}				
									if(commRcvBuff2[2]==FACTORY_RESTORE){//�ָ���������(��&��)0x15
											if(btErrCode==0){
													SendShortCmd(COMM_BT_RESTORE,0);  //�ش���λ����Ϣ���ָ��ɹ�		
											}else{
													SendShortCmd(COMM_BT_RESTORE|0x80,btErrCode);
											}
									}	
							break;
											
							default:									
							break;
						}	
				}
				commStatus2 = COMM_IDLE;
		}
}

//��������ת��
int utoi(u8 nub)
{
	return (int)nub-48;
}


void USART2_IRQHandler(void)                	//����2�жϷ������
{
	static u8 ptrRcv2 = 0;//���ջ�����ָ�룬����������ֽ�����USART_REC_LEN = 10
		
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
		{
				commRcvBuff2[ptrRcv2] =USART_ReceiveData(USART2);	//��ȡ���յ�������
				
				if(ptrRcv2==0){	
					  if(commRcvBuff2[0]<2||commRcvBuff2[0]>BUFF_LEN_MAX2){	//�ж�֡����Ч�ԣ���һ���ֽ�Ϊ�����ֽ����֡����
							  frameLen2 = 0; 
								commStatus2 = COMM_IDLE;							
						}else{
								frameLen2 = commRcvBuff2[0] + 1;
								commStatus2 = COMM_RECEIVING;
						}
				}
				//�����������������ݷ����������������ɹ�����ǰ��෢һ��0x03������--ԭ���ҵ�--������������ʱ����֡�
				if(commStatus2 == COMM_RECEIVING){
						if(ptrRcv2>=frameLen2-1)//zhh bug��frameLen1 -> frameLen2
						{
								ptrRcv2=0;
								commStatus2=COMM_FRAME_READY;  
						}else{
								ptrRcv2++;
						}
				}		
		}		
} 



void USART1_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif

		static u8 ptrRcv1 = 0;//���ջ�����ָ�룬����������ֽ�����USART_REC_LEN = 10
	
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
		{			
				commRcvBuff1[ptrRcv1] =USART_ReceiveData(USART1);	//��ȡ���յ�������
				
				if(ptrRcv1==0){
					  commStatus1 = COMM_RECEIVING;
						switch(commRcvBuff1[0]){
							case COMM_SHAKEHAND:case COMM_READ_ID:case COMM_SENSOR_STATUS: case  COMM_BT_RESTORE:    //֡��Ϊ3�ֽ�
								case COMM_BIND_BEGIN:case COMM_BIND_CHECK:case COMM_UNBIND_SENSOR:case COMM_READ_ANGLE:	//֡��Ϊ3�ֽ�
										 frameLen1 = 3;		break;
								case COMM_READ_PRESSURE:
									   frameLen1 = 4;	  break;
								case COMM_SET_ID:	//֡��Ϊ10�ֽ�
									   frameLen1 = 10;	break;			
								case COMM_BT_TRANSPORT:					//zhh bug δ��͸��ָ����Ĵ���
										 frameLen1 = 15;	break;		//֡���ݶ�Ϊ15�ֽڣ��յ���һ�ֽڲ���ȷ��֡����					 						
								default: 			//��������벻�ԣ�������ڽ��ձ�־������֡����
									   frameLen1 = 0; 
								     commStatus1 = COMM_IDLE;	
										 break;
						}									
				}				
				
				if(commStatus1 == COMM_RECEIVING){
						if(commRcvBuff1[0]==COMM_BT_TRANSPORT && ptrRcv1==1){//��͸��ָ���£��ڶ��ֽ�Ϊ֡��
									frameLen1 = commRcvBuff1[1] + 3;							 //zhh bug δ��͸��ָ����Ĵ���
						}
						if(ptrRcv1>=frameLen1-1)
						{								
								ptrRcv1 = 0;
								commStatus1 = COMM_FRAME_READY;								
						}else{
								ptrRcv1++;
						}
				}		
		}	
}


#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif

#endif	

