#include "led.h"
    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				     //LED1-->PC.13 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					       //根据设定参数初始化GPIOC.13
 GPIO_SetBits(GPIOC,GPIO_Pin_13);						           //PC.13 输出高 LED灭
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				     //LED2-->PC.14 端口配置
 GPIO_Init(GPIOC, &GPIO_InitStructure);					       //根据设定参数初始化GPIOC.14
 GPIO_SetBits(GPIOC,GPIO_Pin_14);						           //PC.14 输出高 LED灭
}
 
void LED_On(void){
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//PC.13 输出低电平 点亮LED
}
void LED_Off(void){
		GPIO_SetBits(GPIOC,GPIO_Pin_13); 		//PC.13 输出高电平 LED灭
}

	
