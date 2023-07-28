#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "led.h"

//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTBʱ��

		//��ʼ�� BT_RES-->GPIOA.15
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
		GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA15

		//��ʼ�� KEY_ON-->GPIOB.10	  ��������
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó����룬Ĭ������	  
		GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.10
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(BT_RES==0||KEY_ON==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(BT_RES==0)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_10);
		  return BT_RES_PRES;
		}
		else if(KEY_ON==1)return KEY_ON_PRES;
	}else if(BT_RES==1&&KEY_ON==0)key_up=1; 	    
 	return 0;// �ް�������
}


u8 Check_WKUP(void) 
{
	u8 t=0;	//��¼���µ�ʱ��
	LED1=1; //�ر�LED
	while(1)
	{
		if(KEY_ON == 1)
		{
			t++;			//�Ѿ������� 
			delay_ms(30);
			if(t>=100)		//���³���3����
			{
				return 1; 	//����3s������
			}
		}else 
		{ 
			//LED2=0;
			//wakeup
			return 0; //���²���3��
		}
	}
} 