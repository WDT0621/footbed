#include "wkup.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//待机唤醒 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
	 
void Sys_Stop(void)
{  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	  //进入停机（STOP）模式 		 
}
//系统进入待机模式
void Sys_Enter_Stop(void)
{			 
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);	//复位所有IO口
	Sys_Stop();
}

/**
  * @brief  停机唤醒后配置系统时钟: 使能 HSE, PLL
  *         并且选择PLL作为系统时钟.
  * @param  None
  * @retval None
  */
void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* 使能 HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* 等待 HSE 准备就绪 */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* 使能 PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* 等待 PLL 准备就绪 */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* 选择PLL作为系统时钟源 */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* 等待PLL被选择为系统时钟源 */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
}
