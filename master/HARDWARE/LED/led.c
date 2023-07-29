#include "led.h"
    
//LED IO初始化
void LED_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA、PB端口时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //开启AFIO复用时钟
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//用PB3，PB4，PA15做普通IO，PA13&14用于SWD调试重映射
	
		/*===============================================LED=================================================*/
		//LED1-->PB.6 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					     
		//LED2-->PB.7 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	           
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		/*===============================================PWR_EN===============================================*/
		//PWR_EN-->PA.0 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	           
		GPIO_Init(GPIOA, &GPIO_InitStructure);			
	
		/*======================================CHRG(新版取消了)================================================*/
		//CHRG-->PA.1 端口配置
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				     
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		   	//上拉输入
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO口速度为50MHz
//		GPIO_Init(GPIOA, &GPIO_InitStructure);		
		
		/*=============================================CHRG_WKUP================================================*/
		//CHRG_WKUP-->PB.11 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
		/*=========================================驱动电阻阵列GPIO端口=======================================*/
		//驱动阵列第1行-->PB.5 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//驱动阵列第2行-->PB.12 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//驱动阵列第3行-->PB.4 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					      
		//驱动阵列第4行-->PB.13 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//驱动阵列第5行-->PB.3 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					      
		//驱动阵列第6行-->PB.14 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					       
		//驱动阵列第7行-->PA.12 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					      
		//驱动阵列第8行-->PB.15 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);							
		//驱动阵列第9行-->PA.11 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				    
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					       
		//驱动阵列第10行-->PA.8 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					       
		//驱动阵列第11行-->PA.10 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					       
		//驱动阵列第12行-->PA.9 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				     
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					      

		GPIO_SetBits(GPIOB,GPIO_Pin_6);						           //PB.6 输出高 LED1灭
		GPIO_SetBits(GPIOB,GPIO_Pin_7);						           //PB.7 输出高 LED2灭
		GPIO_ResetBits(GPIOA,GPIO_Pin_0);						         //PA.0 输出低 PWR不使能
		//初始化不驱动任意电阻
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
 
