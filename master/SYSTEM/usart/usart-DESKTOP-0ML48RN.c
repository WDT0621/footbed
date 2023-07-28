#include "sys.h"
#include "usart.h"	  
#include <stdio.h>
#include "mpu6050.h"  
#include "led.h"
#include "delay.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
	{};//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
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
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
	
	
	
	
	typedef union
{	
	signed short intData;
	unsigned char byteData[2];
}INT_BYTE;

static INT_BYTE testpitch,testroll,testyaw;



void USART1_Init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//使能USART1，GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//使能USART1，GPIOA时钟
	//GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

void 	USART2_Init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //使能USART2，GPIOA时钟
    
	
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART2_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3 

  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口2
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口2 

}

//void 	USART3_Init(u32 bound)
//{
//  //GPIO端口设置
//  GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //使能USART3，GPIOB时钟
//    
//	
//	//USART3_TX   GPIOB.10
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.10
//   
//  //USART3_RX	  GPIOB.11初始化
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.11 

//  //Usart3 NVIC 配置
//  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级2
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//  
//   //USART 初始化设置

//	USART_InitStructure.USART_BaudRate = bound;//串口波特率
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//  USART_Init(USART3, &USART_InitStructure); //初始化串口3
//  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
//  USART_Cmd(USART3, ENABLE);                    //使能串口3

//}

/*void USART3_IRQHandler(void)                	//串口3中断服务程序
{
    
} 
*/
//恢复出厂设置：恢复后，需重新联网后才能和适配器通信
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

void USART2_IRQHandler(void)                	//串口2中断服务程序
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
	 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
		{
			Count = 0;
			SleepFlag = 0;
			Res =USART_ReceiveData(USART2);	//读取接收到的数据
			memory[i]=Res;
			//USART_SendData(USART1,Res);
			if(memory[0]==0x06)					//组网过程（查询在线未绑定传感器网络地址）处理
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
			
		 
		 
		 
		 
		 
			if(memory[0]==0x0F)						//设置ID过程处理
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
		
	
				if(memory[0]==0x09)							//绑定过程处理
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

		
		
		if(memory[0]==0x08)							//解除绑定过程或角度数据传输处理
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
						if(memory[6]==0x66)										//解除绑定过程
						{
							head1[2]=memory[2];
							head1[3]=memory[3];
													delay_ms(20);
							Send_datatoU2(4,head1);
							Send_datatoU2(3,unbindgood);
							message.buwei=0x00;
						}
						if(memory[6]==0x67)										//角度数据传输过程
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
							
							/************for test，格式改变一下，方便串口调试助手显示***********************/							
							/*if(testpitch.intData<0){ 
									jiaodu[2]=0xF0;		//高四位当作符号位，F表示负数，0表示正数
									testpitch.intData=(-1)*testpitch.intData;	//取绝对值
							}else jiaodu[2]=0x00;
							if(testroll.intData<0){
									jiaodu[4]=0xF0;
									testroll.intData=(-1)*testroll.intData;	
							}else jiaodu[4]=0x00;
							if(testyaw.intData<0){
									jiaodu[6]=0xF0;
									testyaw.intData=(-1)*testyaw.intData;
							}else jiaodu[6]=0x00;
							
							//pitch角度的转换
							tint=testpitch.intData;							
							tint2 = tint/1000;		tint = tint%1000;				jiaodu[2] = jiaodu[2] + tint2;
							tint2 = tint/100;		tint = tint%100;					jiaodu[3]=tint2<<4;  
							tint2 = tint/10;		jiaodu[3]=jiaodu[3]+tint2;  
							//roll角度的转换
							tint = testroll.intData;
							tint2 = tint/1000;		tint = tint%1000;				jiaodu[4] = jiaodu[4] + tint2;
							tint2 = tint/100;		tint = tint%100;					jiaodu[5]=tint2<<4;  
							tint2 = tint/10;		jiaodu[5]=jiaodu[5]+tint2;  							
							//yaw角度的转换
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
							GPIO_SetBits(GPIOB,GPIO_Pin_6);	//LED灭							
						}else if(memory[6]==0x68){		//蓝牙从模块恢复出厂设置
							BTRestore();  //发送恢复出厂配置命令
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
				 GPIO_ResetBits(GPIOB,GPIO_Pin_6);		//LED亮							
							
			 }			
		}
  
} 



void USART1_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif

	
	

}


#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif

#endif	