#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "exti.h"
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "key.h"
#include "adc.h"
#include "wkup.h"
#include "timer.h"
#include "getvol.h"
#include <stdio.h>

typedef union
{	
		signed short intData;
		unsigned char byteData[2];
}INT_BYTE;


//static int Rdflag=0;

void Send_datatoU2(int length ,u8 *s)
{
		int i=0;
		for(i=0;i<length;i++)
		{
				while((USART2->SR&0x0080)==0)
				{} ; //�����ݻ�û�б�ת�Ƶ���λ�Ĵ�����ȴ�
				USART2->DR=s[i];//����һ���ֽڵ�����
		}
}
void Send_datatoU1(int length ,u8 *s)
{
		int i=0;
		for(i=0;i<length;i++)
		{
				while((USART1->SR&0x0080)==0)
				{} ; //�����ݻ�û�б�ת�Ƶ���λ�Ĵ�����ȴ�
				USART1->DR=s[i];//����һ���ֽڵ�����
		}
}
//int Getflag(void)
//{
//		if(Rdflag==1)
//		{
//				return 1;
//		}	
//		else
//		{
//				return 0;
//		}	
//}

u8 Count = 0;
u8 SleepFlag;
float pitch,roll,yaw; 
float press_vol[48];
u8 ADC_origin[72];

volatile
int main(void)
{
//		ADC_ConvertedValueLocal[0] = (float) ADC_ConvertedValue[0]/4096*3.3;
//		ADC_ConvertedValueLocal[1] = (float) ADC_ConvertedValue[1]/4096*3.3;
//		ADC_ConvertedValueLocal[2] = (float) ADC_ConvertedValue[2]/4096*3.3;
//		ADC_ConvertedValueLocal[3] = (float) ADC_ConvertedValue[3]/4096*3.3;
		extern uint16_t ADC_ConvertedValue[Channel_Num];
		float bat_vol;
		INT_BYTE testpitch,testroll,testyaw;
		u8 Reset[3]={0x02,0xC0,0x15};						//�ָ���������
		u8 Getadr[3]={0x02,0xC0,0x0B};
		u8 temp[3];
		u8 Res;
		u8 ID=0x01;
		u8 jiaodu[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x88};
		u8 head[6]={0x0c,0xc2,0x00,0x01,0x00,0x00};
		u16 adcx;
		//float ADCtemp;
		int a=0;
		int j=0;
		int b=0;
    
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		USART2_Init(115200);    //����2��ʼ��Ϊ115200:����ģ��
		delay_init();	        	//��ʱ��ʼ�� 
		LED_Init();		  				//��ʼ����LED���ӵ�Ӳ���ӿ�
		KEY_Init();
		ADC1_Init();
		EXTIX_Init();
		BTPwrSet();
		MPU_Init();						//��ʼ��MPU6050
		mpu_dmp_init();
		LED1 = 0;							//LED1��	
		while(mpu_dmp_init())
		{	
				delay_ms(20);			
		}
		LED1 = 1;delay_ms(200);//LED1��˸	
		LED1 = 0;delay_ms(80);
		LED1 = 1;delay_ms(200); 
		LED1 = 0;delay_ms(80); 
		LED1 = 1;delay_ms(200); 
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//��������1�����ж�
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//��������2�����ж�				
		//BTRestore();			//����ģ��ָ��������ã�for debug
		PWR_EN = 1;								//ʹ����ѹоƬ����3.3V����
		StopFlag = 0;							//�ػ���־
		SleepFlag = 0;						//�͹��ı�־
		delay_ms(500);
		TIM3_Int_Init(29999,7199);//10Khz�ļ���Ƶ�ʣ�������30000Ϊ3s  ÿ3S��count+1
		LED1 = 0;									//LED1��
		while(1)									//��ѭ��
		{	
				if(Count > 20)				//1min�޲����͹�����λ
				{
						Count = 0;
						SleepFlag =1;
				}
				if(SleepFlag == 1)		//����͹��ģ��ں�ֹͣ���������	
				{
						LED1 = 1;
						LED2 = 1;
						TIM_Cmd(TIM3, DISABLE);  
						__WFI();					//wfi���жϣ�wfe���¼������Ѻ�������һ�п�ʼ					
				}
				if(StopFlag == 1)			//ͣ����ֻ��ͨ����GPIO�����ϲ�����ƽ���ش����ⲿ�ж�
				{											//flash��sram���ڹ���
						LED1 = 1;
						LED2 = 1;
						Sys_Enter_Stop();
				}	
				delay_ms(100);
				bat_vol = (float)ADC_ConvertedValue[4]/4096*3.3*2.25;
				if(CHRG_WKUP == 1)//======================================������ˣ����ж����ڳ��
				{
						Count = 0;
						if(bat_vol > CHRG_stage1)
						{
								if(bat_vol > FULLCHRG)
								{
										LED1 = 0;         //������  1��2����
										LED2 = 1;
								}
								else
								{
										LED1 = 0;         //���׶�1  ��������
										LED2 = 0;
								}
						}
						else
						{
								LED1 = 1;
								LED2 = 0;            //���׶�0	2��1����
						}
				}
				else
				{
						if(bat_vol < LOWPWR)
						{
								LED1 = 1;
								delay_ms(80); LED2 = 0;	 	//LED2��
								delay_ms(80); LED2 = 1;		 	//LED2��
								delay_ms(80); LED2 = 0;	 	//LED2��
								LED2 = 1;										//LED2��
						}
						else
						{
								LED1 = 0;
								LED2 = 1;
						}
				}
				if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)                        
				{
						delay_ms(100);
				}
				getvoltage();
		}	
		/*				Send_datatoU2(3,Getadr);
		a=0;
		while(1)
		{	
		j=0;
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)//�����ݼĴ����ǿ�
		{
		Res=USART_ReceiveData(USART2);	
		if((a==0)&&(Res==0x05))
		{
		a=1;
		j=1;							 
		}
		if((a==1)&&(Res==0x40))
		{
		a=2;
		j=1;	
		}	
		if((a==2)&&(Res==0x0B))
		{
		a=3;
		j=1;	
		} 	
		if((a==3)&&(Res==0x01))
		{
		a=4;
		j=1;	
		}
		if((a==4)&&(Res==0x00))
		{
		a=5;
		j=1;	

		}
		if((a==5)&&(Res!=0x00))
		{
		a=0;
		j=1;

		break;
		}
		if(a!=0&&j==0)
		{
		a=0;
		Send_datatoU2(3,Getadr);
		}
		}	
		}*/



//while(1)
//{
//		delay_ms(2);
//		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)                        
//		{
//			LED=~LED;
//		}
//		testpitch.intData = (int)(pitch*10);
//		testroll.intData = (int)(roll*10);
//		testyaw.intData = (int)(yaw*10);
//		head[4]=0x67;
//		head[5]=ID;
//		jiaodu[0]=testpitch.byteData[1];
//		jiaodu[1]=testpitch.byteData[0];
//		jiaodu[2]=testroll.byteData[1];
//		jiaodu[3]=testroll.byteData[0];
//		jiaodu[4]=testyaw.byteData[1];
//		jiaodu[5]=testyaw.byteData[0];
//		jiaodu[6]=0x88;
//		Send_datatoU1(6,head);
//		Send_datatoU1(7,jiaodu);
//		delay_ms(100);
//		//printf("%d,%x,%x\r\n",testpitch.intData,testpitch.byteData[1],testpitch.byteData[0]);
//		//printf("%f,%f,%f\r\n",pitch,roll,yaw);
//} 	
}
 