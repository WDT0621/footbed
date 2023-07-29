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
	//u8 Reset[3]={0x02,0xC0,0x15};					//�ָ���������
    //u8 del[6] = {0x05,0xC1,0x00,0x32,0x80,0x49};
//	u8 Res;
//	int a=0;
	int i=0;

			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
			USART1_Init(115200);	 	//����1��ʼ��Ϊ115200 USB/����
			USART2_Init(115200);    //����2��ʼ��Ϊ9600 ������ģ�� 10M
		//	USART3_Init(9600);   	//����3��ʼ��Ϊ9600  ����ɨƽ��ģ��
			delay_init();	       		//��ʱ��ʼ�� 
			LED_Init();		  				//��ʼ����LED���ӵ�Ӳ���ӿ�
			LED_On();
			//SendToPC(6,del); 				 
			//SendToPC(3,Reset);   //�ϵ��������λ��������
			//BTSendCfg(DEVICE_RESET);
			//BTSendCfg(FACTORY_RESTORE);//�ָ��������� for test
			delay_ms(50);			LED_Off();
			
			STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,15);   //��0~7�ֽ���ID,8~14�ֽ��Ǵ����������ַ
			for(i=0;i<8;i++)                                    //��ȡflash
			{
					suitMAC[i] = datatemp[i];      //ID����
			}			

			//��������ַ��ʼ��
			if(datatemp[i+8]==0xFF){  
					for(i=0;i<7;i++){
							datatemp[i+8] = 0;
							bindinf[i] = 0;
					}
					STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)datatemp,15);   //���󶨵������ַ��ʼ��Ϊ0
			}else{
					for(i=0;i<7;i++) bindinf[i]=datatemp[i+8];				
			}
			
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������1�����ж�
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������2�����ж�
			
			delay_ms(50);LED_On();
			delay_ms(50);LED_Off();
			delay_ms(10);	LED_On();
			delay_ms(10);LED_Off();
			delay_ms(10);	LED_On();
						
			while(1)
			{
					 FrameParse1();					//USART1ͨ�Ű�����--��λ��ͨ��
				   FrameParse2();					//USART2ͨ�Ű�����--����ģ��ͨ�� zhh bug --û����
			}					

			 	
}
 


