#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED PCout(13)

void LED_Init(void);	//��ʼ��
void LED_On(void);		//LED��
void LED_Off(void);		//LEDϨ
		 				    
#endif
