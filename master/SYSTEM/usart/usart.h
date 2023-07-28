#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	
extern float pitch,roll,yaw;
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern u8 Count;
//����봮���жϽ��գ��벻Ҫע�����º궨��
void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void USART3_Init(u32 bound);
void BTRestore();
void BTPwrSet();
u8 GetCheckSum(u8 inLen,u8 *inBuff);
u8 CheckSumTest(u8 inLen,u8 *inBuff);
#endif


