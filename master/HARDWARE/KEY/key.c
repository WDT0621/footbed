#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "led.h"

//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTB时钟

		//初始化 BT_RES-->GPIOA.15
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA15

		//初始化 KEY_ON-->GPIOB.10	  下拉输入
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成输入，默认上拉	  
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(BT_RES==0||KEY_ON==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(BT_RES==0)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_10);
		  return BT_RES_PRES;
		}
		else if(KEY_ON==1)return KEY_ON_PRES;
	}else if(BT_RES==1&&KEY_ON==0)key_up=1; 	    
 	return 0;// 无按键按下
}


u8 Check_WKUP(void) 
{
	u8 t=0;	//记录按下的时间
	LED1=1; //关闭LED
	while(1)
	{
		if(KEY_ON == 1)
		{
			t++;			//已经按下了 
			delay_ms(30);
			if(t>=100)		//按下超过3秒钟
			{
				return 1; 	//按下3s以上了
			}
		}else 
		{ 
			//LED2=0;
			//wakeup
			return 0; //按下不足3秒
		}
	}
} 