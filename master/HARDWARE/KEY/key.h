#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define WK_UP PAin(0)	//PA0  WK_UP

#define BT_RES  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)//读取蓝牙复位按键
#define KEY_ON  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//读取按键1
//#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键2 
//#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 

 

#define BT_RES_PRES 	1	//KEY0按下
#define KEY_ON_PRES	2	//KEY1按下
//#define KEY2_PRES	3	//KEY2按下
//#define WKUP_PRES   4	//KEY_UP按下(即WK_UP/KEY_UP)


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数	
u8 Check_WKUP(void);
#endif
