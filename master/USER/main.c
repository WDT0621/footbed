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
				{} ; //当数据还没有被转移到移位寄存器则等待
				USART2->DR=s[i];//发送一个字节的数据
		}
}
void Send_datatoU1(int length ,u8 *s)
{
		int i=0;
		for(i=0;i<length;i++)
		{
				while((USART1->SR&0x0080)==0)
				{} ; //当数据还没有被转移到移位寄存器则等待
				USART1->DR=s[i];//发送一个字节的数据
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
		u8 Reset[3]={0x02,0xC0,0x15};						//恢复出厂设置
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
    
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		USART2_Init(115200);    //串口2初始化为115200:蓝牙模块
		delay_init();	        	//延时初始化 
		LED_Init();		  				//初始化与LED连接的硬件接口
		KEY_Init();
		ADC1_Init();
		EXTIX_Init();
		BTPwrSet();
		MPU_Init();						//初始化MPU6050
		mpu_dmp_init();
		LED1 = 0;							//LED1亮	
		while(mpu_dmp_init())
		{	
				delay_ms(20);			
		}
		LED1 = 1;delay_ms(200);//LED1闪烁	
		LED1 = 0;delay_ms(80);
		LED1 = 1;delay_ms(200); 
		LED1 = 0;delay_ms(80); 
		LED1 = 1;delay_ms(200); 
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启串口1接受中断
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启串口2接受中断				
		//BTRestore();			//蓝牙模块恢复出厂设置：for debug
		PWR_EN = 1;								//使能稳压芯片持续3.3V供电
		StopFlag = 0;							//关机标志
		SleepFlag = 0;						//低功耗标志
		delay_ms(500);
		TIM3_Int_Init(29999,7199);//10Khz的计数频率，计数到30000为3s  每3S后count+1
		LED1 = 0;									//LED1灭
		while(1)									//主循环
		{	
				if(Count > 20)				//1min无操作低功耗置位
				{
						Count = 0;
						SleepFlag =1;
				}
				if(SleepFlag == 1)		//进入低功耗，内核停止，外设继续	
				{
						LED1 = 1;
						LED2 = 1;
						TIM_Cmd(TIM3, DISABLE);  
						__WFI();					//wfi等中断，wfe等事件，唤醒后程序从下一行开始					
				}
				if(StopFlag == 1)			//停机，只能通过在GPIO引脚上产生电平边沿触发外部中断
				{											//flash和sram还在供电
						LED1 = 1;
						LED2 = 1;
						Sys_Enter_Stop();
				}	
				delay_ms(100);
				bat_vol = (float)ADC_ConvertedValue[4]/4096*3.3*2.25;
				if(CHRG_WKUP == 1)//======================================这里改了，能判断正在充电
				{
						Count = 0;
						if(bat_vol > CHRG_stage1)
						{
								if(bat_vol > FULLCHRG)
								{
										LED1 = 0;         //充满电  1亮2不亮
										LED2 = 1;
								}
								else
								{
										LED1 = 0;         //充电阶段1  两个都亮
										LED2 = 0;
								}
						}
						else
						{
								LED1 = 1;
								LED2 = 0;            //充电阶段0	2亮1不亮
						}
				}
				else
				{
						if(bat_vol < LOWPWR)
						{
								LED1 = 1;
								delay_ms(80); LED2 = 0;	 	//LED2亮
								delay_ms(80); LED2 = 1;		 	//LED2灭
								delay_ms(80); LED2 = 0;	 	//LED2亮
								LED2 = 1;										//LED2灭
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
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)//读数据寄存器非空
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
 