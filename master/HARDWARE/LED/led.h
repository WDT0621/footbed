#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED1 PBout(6)
#define LED2 PBout(7)
#define PWR_EN PAout(0)
#define CHRG_WKUP PBin(11)

#define DRIVE_ROW1 GPIO_SetBits(GPIOB,GPIO_Pin_5);
#define UNDRIVE_ROW1 GPIO_ResetBits(GPIOB,GPIO_Pin_5);
#define DRIVE_ROW2 GPIO_SetBits(GPIOB,GPIO_Pin_12);
#define UNDRIVE_ROW2 GPIO_ResetBits(GPIOB,GPIO_Pin_12);
#define DRIVE_ROW3 GPIO_SetBits(GPIOB,GPIO_Pin_4);
#define UNDRIVE_ROW3 GPIO_ResetBits(GPIOB,GPIO_Pin_4);
#define DRIVE_ROW4 GPIO_SetBits(GPIOB,GPIO_Pin_13);
#define UNDRIVE_ROW4 GPIO_ResetBits(GPIOB,GPIO_Pin_13);
#define DRIVE_ROW5 GPIO_SetBits(GPIOB,GPIO_Pin_3);
#define UNDRIVE_ROW5 GPIO_ResetBits(GPIOB,GPIO_Pin_3);
#define DRIVE_ROW6 GPIO_SetBits(GPIOB,GPIO_Pin_14);
#define UNDRIVE_ROW6 GPIO_ResetBits(GPIOB,GPIO_Pin_14);
#define DRIVE_ROW7 GPIO_SetBits(GPIOA,GPIO_Pin_12);
#define UNDRIVE_ROW7 GPIO_ResetBits(GPIOA,GPIO_Pin_12);
#define DRIVE_ROW8 GPIO_SetBits(GPIOB,GPIO_Pin_15);
#define UNDRIVE_ROW8 GPIO_ResetBits(GPIOB,GPIO_Pin_15);
#define DRIVE_ROW9 GPIO_SetBits(GPIOA,GPIO_Pin_11);
#define UNDRIVE_ROW9 GPIO_ResetBits(GPIOA,GPIO_Pin_11);
#define DRIVE_ROW10 GPIO_SetBits(GPIOA,GPIO_Pin_8);
#define UNDRIVE_ROW10 GPIO_ResetBits(GPIOA,GPIO_Pin_8);
#define DRIVE_ROW11 GPIO_SetBits(GPIOA,GPIO_Pin_10);
#define UNDRIVE_ROW11 GPIO_ResetBits(GPIOA,GPIO_Pin_10);
#define DRIVE_ROW12 GPIO_SetBits(GPIOA,GPIO_Pin_9);
#define UNDRIVE_ROW12 GPIO_ResetBits(GPIOA,GPIO_Pin_9);

void LED_Init(void);//≥ı ºªØ

		 				    
#endif
