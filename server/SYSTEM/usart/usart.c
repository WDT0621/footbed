#include "sys.h"
#include "usart.h"	  
#include <stdio.h>
#include "mpu6050.h"  
#include "led.h"
#include "delay.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "stmflash.h"
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

u8 bindinf[7];		//传感器绑定状态
u8 pCurrent;  		//当前位置

//串口1变量
u8 commStatus1 = COMM_IDLE; 			//串口1通信状态
u8 commRcvBuff1[BUFF_LEN_MAX1];  	//串口1接收缓冲区
u8 commSendBuff1[BUFF_LEN_MAX1];  //串口1发送缓冲区
u8 frameLen1;           					//串口1帧长

//串口2变量
u8 commStatus2 = COMM_IDLE;				//串口2通信状态
u8 commRcvBuff2[BUFF_LEN_MAX2];  	//串口2接收缓冲区---最大长度20
u8 commSendBuff2[BUFF_LEN_MAX2];  //串口2发送缓冲区
u8 frameLen2;           					//串口2帧长

//蓝牙操作
u8 msgCtlType;			//控制消息类型
u8 msgOpCode;				//操作码
u8 msgRcvType;			//收到的消息类型
u16 addrSource;			//原地址
u16 addrTarget;			//目标地址
u8 btMode = 0;				//蓝牙通信模式：0表示一般模式；1表示透传模式
u8 netAddStatus = NETADD_IDLE;  //组网进程状态
u8 btErrCode = 0;		//蓝牙错误码
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

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目

static u8 SaveBuffer[15];//用于Flash存取
u8 suitMAC[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

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

//发送数据到上位机--通过串口1
void SendToPC(u8 length ,u8 *s)
{
	int i=0;
	    delay_ms(2);//QQQ为什么需要延时？---尚未弄清原因
      for(i=0;i<length;i++)
      {
					while((USART1->SR&0x0080)==0); //当数据还没有被转移到移位寄存器则等待
					USART1->DR=s[i];//发送一个字节的数据
      }
}

//发送数据到蓝牙模块（包括与传感器通信）-通过串口2
void BTSendData(int length ,u8 *s)
{
	int i=0;
			delay_ms(2);	//？？？为什么需要延时？
      for(i=0;i<length;i++)
      {
					while((USART2->SR&0x0080)==0); //当数据还没有被转移到移位寄存器则等待
					USART2->DR=s[i];//发送一个字节的数据
      }
}
//发送蓝牙命令--配置命令，只适用于3字节短指令
//输入：inOpCode：操作码
void BTSendCfg(u8 inOpCode)
{
		commSendBuff2[0]=2;
	  commSendBuff2[1]=CONTROL_MSG_CONFIG;
	  commSendBuff2[2]=inOpCode;
		BTSendData(3,commSendBuff2);
}
//发送蓝牙命令--组网地址设置命令
//输入：inAddr--下一次组网时的将要分配给传感器的地址
void BTSetNextAddr(u16 inAddr)
{
		commSendBuff2[0]=4;
	  commSendBuff2[1]=CONTROL_MSG_CONFIG;
	  commSendBuff2[2]=NEXT_ADDR_SET;		//操作码
	  commSendBuff2[3]=inAddr/256;
	  commSendBuff2[4]=inAddr%256;	
		BTSendData(5,commSendBuff2);
}
//发送蓝牙命令--SIG命令：删除节点命令---该命令不支持，暂未使用
//输入：inNodeAddr：节点网络地址
void BTDelNode(u16 InNodeAddr)
{
		commSendBuff2[0]=5;
	  commSendBuff2[1]=CONTROL_MSG_SIG;  //用SIG命令删除节点
	  commSendBuff2[2]=InNodeAddr/256;
	  commSendBuff2[3]=InNodeAddr%256;
	  commSendBuff2[4]=0x80;
	  commSendBuff2[5]=0x49;
		BTSendData(6,commSendBuff2);
}
//发送蓝牙命令到传感器--通过透传消息命令（注：蓝牙模块的透传命令--与上位机的透传不同）
//输入：inCmdCode：命令码；inPosition：传感器位置；inNodeAddr：传感器网络地址
void BTCmdToSensor(u8 inCmdCode,u8 inPosition,u16 InNodeAddr)
{
		commSendBuff2[0]=6;
	  commSendBuff2[1]=CONTROL_MSG_TRANS;  
	  commSendBuff2[2]=InNodeAddr/256;
	  commSendBuff2[3]=InNodeAddr%256;
	  commSendBuff2[4]=inCmdCode;
	  commSendBuff2[5]=inPosition;
	  commSendBuff2[6]=commSendBuff2[4] + commSendBuff2[5];//校验和
		BTSendData(7,commSendBuff2);
}
//校验和计算
//输入：inLen：帧长度；inBuff[]：缓冲区
//     注意：inLen包括校验和字节的长度
//					但是计算时并不包括最后一个校验和字节
u8 GetCheckSum(u8 inLen,u8 *inBuff)
{
	u8 i;
	u8 tcheckSum = 0;
	
		for(i=0;i<inLen-1;i++)
		{
			tcheckSum = tcheckSum+inBuff[i];
		}
		return tcheckSum;
}

//校验和判断
//输入：inLen：帧长度；buff[]：缓冲区
//输出：True：校验正确；False：校验错误
u8 CheckSumTest(u8 inLen,u8 *inBuff)
{		
		return GetCheckSum(inLen,inBuff)==inBuff[inLen-1];
}
//发送短命令（3字节）到上位机：命令码+参数+校验和
//输入：inCmdCode：命令码；inParam：1字节参数
void SendShortCmd(u8 inCmdCode,u8 inParam){
u8 tBuff[3];
	
	  tBuff[0] = inCmdCode; //最高位置1，作为返回的错误命令码
	  tBuff[1] = inParam;
	  tBuff[2] = GetCheckSum(3,tBuff);
    SendToPC(3,tBuff);
}


//解析收到的通信帧
void FrameParse1(void){

		u8 j;  					//局部循环变量
		u8 cmdCode;			//命令码
		u8 cmdCodeErr;	//命令码对应的错误命令码
		
    if(commStatus1 == COMM_FRAME_READY){	
				cmdCode = commRcvBuff1[0];  
				cmdCodeErr = cmdCode|0x80;
				if(!CheckSumTest(frameLen1,commRcvBuff1))//校验和判断
				{
						SendShortCmd(cmdCodeErr,COMM_CHECKSUM_ERR);					
				}else{
						switch(cmdCode){
						case 	COMM_SHAKEHAND:		//PC输入握手指令--0x60 --   to test in Hex：60 55 B5
									if(commRcvBuff1[1]!=0x55){
											SendShortCmd(cmdCodeErr,COMM_SHAKEHAND_ERR);
									}else{							
											SendShortCmd(cmdCode,0xAA);
									}
						break;				
						case COMM_SET_ID:				//设置设备ID -0x61-- to test in Hex：61 20 21 01 26 00 01 01 00 CB
									for(j=0;j<8;j++)
									{
										suitMAC[j]=commRcvBuff1[j+1];        //2~8字节为设备ID，存入suitMAC
										SaveBuffer[j] = suitMAC[j];//将需要的数据存入缓存变量
									}
									STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);
									STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);          //再读出进行检验
									for(j=0;j<8;j++)                                  
									{
											if(suitMAC[j] != SaveBuffer[j])break;
									}
									if (j<8){
											SendShortCmd(cmdCodeErr,COMM_FLASH_ERR);
									}else{
											SendShortCmd(cmdCode,COMM_NO_ERR);
									}
						break;
						case COMM_READ_ID:				//读取设备ID-0x62--to test in Hex：62 00 62
									STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);    		//0~7位是ID,8~14位是绑定地址
									for(j=0;j<8;j++) suitMAC[j] = SaveBuffer[j];									
									commSendBuff1[0] = COMM_READ_ID;
									for(j=0;j<8;j++) commSendBuff1[j+1]=suitMAC[j];
									commSendBuff1[9] = GetCheckSum(10,commSendBuff1);			//计算校验和
									SendToPC(10,commSendBuff1);				//发送ID值
						break;	
						case COMM_SENSOR_STATUS:				//查询绑定状态-0x63--to test in Hex：63 00 63
									STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);          //0~7位是ID,8~14位是绑定地址
									for(j=0;j<7;j++) bindinf[j] = SaveBuffer[j+8];																		
									commSendBuff1[0]=0X63;
									commSendBuff1[1]=0X07;
									for(j=0;j<7;j++) commSendBuff1[j+2]=bindinf[j];
									commSendBuff1[9] = GetCheckSum(10,commSendBuff1);//计算校验和
									SendToPC(10,commSendBuff1);				//发送绑定状态值
						break;				
						case COMM_BIND_BEGIN:		//绑定传感器   -0x64--to test in Hex：64 01 65  (绑定位置1的传感器)
									if(commRcvBuff1[1]==0||commRcvBuff1[1]>7){		//给的传感器位置判断
											SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);				
									}else{
											STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);          //0~7位是ID,8~14位是绑定地址
											for(j=0;j<7;j++)	bindinf[j] = SaveBuffer[j+8];								
											j = commRcvBuff1[1]-1;		//通信命令参数commRcvBuff1[1]为传感器位置
											if(bindinf[j]==0){      	//判断是否已经绑定有传感器
													SendShortCmd(cmdCode,0);        
													BTSendCfg(NETWORK_ADDIN);				//启动组网指令
													pCurrent = commRcvBuff1[1];     //保存绑定位置
												  netAddStatus = NET_ADDING;			//正在组网
											}else{    //如果已经绑定传感器，返回错误
													SendShortCmd(cmdCodeErr,COMM_OTHER_BOUND_ERR);			
											}
									}
						break;
						case COMM_BIND_CHECK:							//0x65查询绑定进程
									if(netAddStatus==NETADD_IDLE){   					//无绑定过程
											SendShortCmd(cmdCodeErr,BT_NET_ADDING_NULL);								
							    }else if(netAddStatus==NET_ADDING){   		//绑定未完成
											SendShortCmd(cmdCodeErr,BT_NET_ADDING_ONGO);
									}else if(netAddStatus==NETADD_FLASH_FAILURE){   		//保存FLASH时发生错误
											SendShortCmd(cmdCodeErr,BT_NET_ADDING_FLASH_ERR);
									}else	if(netAddStatus==NETADD_SUCCESS){	//绑定成功
											SendShortCmd(cmdCode,bindinf[pCurrent-1]);	//回传PC：带回参数为传感器网络地址（收到消息时bindinf[]已更新）
											netAddStatus = NETADD_IDLE;
									}else{																		//发生错误
										  SendShortCmd(cmdCodeErr,btErrCode); 			//发送蓝牙错误码
											netAddStatus = NETADD_IDLE;
									}									
						break;	
						case COMM_UNBIND_SENSOR:	//解绑传感器  -0x66--to test in Hex：66 01 67  (解绑位置1的传感器)
									if(commRcvBuff1[1]==0||commRcvBuff1[1]>7){		
											SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);				//给的传感器位置错误
									}else{																			
											STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15); 
											for(j=0;j<7;j++)	bindinf[j] = SaveBuffer[j+8];								
											pCurrent = commRcvBuff1[1];							//绑定的位置
											if(bindinf[pCurrent-1]!=0){   					//如果绑定有传感器--可解绑
														//发送命令给传感器：蓝牙模块恢复出厂设置
												  BTCmdToSensor(COMM_BT_RESTORE,commRcvBuff1[1],(u16)bindinf[pCurrent-1]); 
														//修改保存到FLASH
													bindinf[pCurrent-1] = 0;    				//改为0，表示没绑定传感器
													SaveBuffer[pCurrent+7] = 0;	
													STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);	
														//回传PC：告知解绑成功
													SendShortCmd(COMM_UNBIND_SENSOR,pCurrent);																					
											}else{ 									
													SendShortCmd(cmdCodeErr,COMM_NO_BOUND_ERR);	//没有绑定有传感器--错误
											}
									}
						break;
						case COMM_BT_RESTORE:			//0x68		蓝牙模块恢复出厂配置									
								 BTSendCfg(FACTORY_RESTORE);
						break;		
						case COMM_READ_PRESSURE:			//0x69		读取压力									
								if(commRcvBuff1[1]==0||commRcvBuff1[1]>0x07)
								{
										SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);										
								}else{
										j = commRcvBuff1[1]-1;		//commRcvBuff1[1]为传感器位置
										if(bindinf[j]!=0){      	//如果绑定有传感器
												
												commSendBuff2[0]=7;
												commSendBuff2[1]=CONTROL_MSG_TRANS;  
												commSendBuff2[2]=(u16)bindinf[j]/256;
												commSendBuff2[3]=(u16)bindinf[j]%256;
												commSendBuff2[4]=0x69;
												commSendBuff2[5]=commRcvBuff1[1];
												commSendBuff2[6]=commRcvBuff1[2];
												commSendBuff2[7]=commSendBuff2[6] + commSendBuff2[5] + commSendBuff2[4];//校验和
												BTSendData(8,commSendBuff2);
										}else{ 									
												SendShortCmd(cmdCodeErr,COMM_NO_BOUND_ERR);		 //该位置没有绑定传感器
										}
								}								 
						break;						
						case COMM_BT_TRANSPORT:		//上位机端使用的蓝牙透传指令					
								 btMode = 1;
								 BTSendData(commRcvBuff1[1]+1,commRcvBuff1+1);//第2字节+1为帧长，从第2字节开始发送
						break;
						case COMM_READ_ANGLE:		//读传感器的角度数据
							  //QQQ超时需要加吗？
								if(commRcvBuff1[1]==0||commRcvBuff1[1]>0x07)
								{
										SendShortCmd(cmdCodeErr,COMM_POSITION_ERR);										
								}else{
										j = commRcvBuff1[1]-1;		//commRcvBuff1[1]为传感器位置
										if(bindinf[j]!=0){      	//如果绑定有传感器
												BTCmdToSensor(cmdCode,commRcvBuff1[1],(u16)bindinf[j]);  //命令发送给传感器
										}else{ 									
												SendShortCmd(cmdCodeErr,COMM_NO_BOUND_ERR);		 //该位置没有绑定传感器
										}
								}
						default:								
						break;
						}
				}
				commStatus1 = COMM_IDLE;	//处理完成，通信状态设为空闲
				LED_Off();	delay_ms(5);	LED_On();//LED闪烁，指示收到命令
		}			
}
//对蓝牙模块参数消息的处理
//输入：操作码 inOpCode；数据 inData
void BTDealCfg(u8 inOpCode,u8 * inData)
{
	u16 i;
		switch(inOpCode){
			case  NETKEY_SET:		  			//配置网络密匙（只用于主模块）0x00	
			break;
			case  NETKEY_RD:		  			//读网络密匙（主）0x01	
			break;
			case  APPKEY_SET:		  			//配置应用密匙（主）0x02
			break;
			case  APPKEY_RD:		  			//读应用密匙（主）0x03	
			break;
			case  NEXT_ADDR_SET:  			//设置下次的组网地址（主）0x04	
			break;
			case  NEXT_ADDR_RD:	  			//读下次的组网地址（主）0x05
			break;			
			case  NETWORK_ADDIN: 				//设备入网（主）0x09	
			      STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);    
						for(i=0;i<7;i++)	bindinf[i] = SaveBuffer[i+8];												
						i = pCurrent - 1;    
						bindinf[i] = inData[7];    				//传感器网络地址
						SaveBuffer[i+8] = bindinf[i];	
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);//存入FLASH中			
						STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15); //再读出来检验是否保存正确
						for(i=0;i<7;i++){	
								if(bindinf[i] != SaveBuffer[i+8])	break;			
						}
						if(i>=7)
								netAddStatus = NETADD_SUCCESS;			//组网成功，等待查询			
						else
								netAddStatus = NETADD_FLASH_FAILURE;
			break;			
			default:
					//QQQ错误参数
			break;
		}
}	

//解包串口2（蓝牙模块）收到的通信帧
void FrameParse2(void){
u16 i;
u16 tFrameLen;
	
    if(commStatus2 == COMM_FRAME_READY){	
				if(btMode==1){	//收到的蓝牙命令打包后发送到上位机
					  commSendBuff1[0]=COMM_BT_TRANSPORT;
					  tFrameLen = commRcvBuff2[0]+1;		//zhh bug commSendBuff2->commRcvBuff2
					  for(i=0;i<tFrameLen;i++){
								commSendBuff1[i+1]=commRcvBuff2[i];
						}
						tFrameLen = tFrameLen + 2;
						commSendBuff1[tFrameLen-1] = GetCheckSum(tFrameLen,commSendBuff1);//计算校验和
						SendToPC(tFrameLen,commSendBuff1);  //发送到上位机  zhh bug commSendBuff2 ->commSendBuff1
						btMode = 0;
				}else{			//正常状态的蓝牙消息处理
						msgCtlType = commRcvBuff2[1];
						switch(msgCtlType){					
							case RESPONSE_MSG_CONFIG:	//配置参数消息  -0x40-				
									BTDealCfg(commRcvBuff2[2],commRcvBuff2 + 3);	//处理参数消息
							break;
							case RESPONSE_MSG_SIG:		//SIG消息 -0x41----目前只处理删除节点消息---经测试，该命令不支持!
									/*if(commRcvBuff2[0]==7&&commRcvBuff2[6]==0x80 && commRcvBuff2[7]==0x4A){//删除节点成功：收到的长度为7，且最后2字节内容正确									
											STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);    
											for(i=0;i<7;i++)	bindinf[i] = SaveBuffer[i+8];
											if(pCurrent>0 && pCurrent<=0x07 && bindinf[pCurrent-1]!=0){
													bindinf[pCurrent-1] = 0;    	//改为0，表示没绑定传感器
													SaveBuffer[pCurrent+7] = 0;	
													STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)SaveBuffer,15);		//存入FLASH中												  
													SendShortCmd(COMM_UNBIND_SENSOR,pCurrent);							//回传PC：解绑成功
											}
									}	*/								
							break;
							case RESPONSE_MSG_TRANS:	//透传消息 -0x42--目前只处理读角度和压力传感器消息
										if(commRcvBuff2[0]==14 && commRcvBuff2[6]==COMM_READ_ANGLE)
										{										  
											if(!CheckSumTest(9,commRcvBuff2+6))
											{//校验和判断											
													SendShortCmd(COMM_READ_ANGLE|0x80,COMM_CHECKSUM_ERR);	
											}
											else
											{
													SendToPC(9,commRcvBuff2+6);	//回传角度数据给上位机：帧格式在传感器端已打包
											}	
										}
										if(commRcvBuff2[0]==33 && commRcvBuff2[6]==COMM_READ_PRESSURE)
										{										  
											if(!CheckSumTest(28,commRcvBuff2+6))
											{//校验和判断											
													SendShortCmd(COMM_READ_PRESSURE|0x80,COMM_CHECKSUM_ERR);	
											}
											else
											{
													SendToPC(28,commRcvBuff2+6);	//回传压力数据给上位机：帧格式在传感器端已打包
											}	
										}
//										if(commRcvBuff2[0]==57 && commRcvBuff2[6]==COMM_READ_PRESSURE)
//										{										  
//											if(!CheckSumTest(52,commRcvBuff2+6))
//											{//校验和判断											
//													SendShortCmd(COMM_READ_PRESSURE|0x80,COMM_CHECKSUM_ERR);	
//											}
//											else
//											{
//													SendToPC(52,commRcvBuff2+6);	//回传压力数据给上位机：帧格式在传感器端已打包
//											}	
//										}
							break;
							case RESPONSE_MSG_ERR:	//产生错误-0x43-  here 也可能有非错误信息：恢复出厂和重启								  
									btErrCode = commRcvBuff2[3];    //保存蓝牙错误码
							    if(commRcvBuff2[2]==NETWORK_ADDIN){//如果是绑定传感器过程（即组网过程）
										   netAddStatus = NETADD_FAILURE;
									}
									if(commRcvBuff2[2]==DEVICE_RESET){ //重启蓝牙模块(主&从)0x14									
											if(btErrCode==0){
													//here 重启蓝牙模块成功
											}
									}				
									if(commRcvBuff2[2]==FACTORY_RESTORE){//恢复出厂设置(主&从)0x15
											if(btErrCode==0){
													SendShortCmd(COMM_BT_RESTORE,0);  //回传上位机消息：恢复成功		
											}else{
													SendShortCmd(COMM_BT_RESTORE|0x80,btErrCode);
											}
									}	
							break;
											
							default:									
							break;
						}	
				}
				commStatus2 = COMM_IDLE;
		}
}

//数据类型转换
int utoi(u8 nub)
{
	return (int)nub-48;
}


void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	static u8 ptrRcv2 = 0;//接收缓冲区指针，不大于最大字节数：USART_REC_LEN = 10
		
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
		{
				commRcvBuff2[ptrRcv2] =USART_ReceiveData(USART2);	//读取接收到的数据
				
				if(ptrRcv2==0){	
					  if(commRcvBuff2[0]<2||commRcvBuff2[0]>BUFF_LEN_MAX2){	//判断帧长有效性：第一个字节为除首字节外的帧长度
							  frameLen2 = 0; 
								commStatus2 = COMM_IDLE;							
						}else{
								frameLen2 = commRcvBuff2[0] + 1;
								commStatus2 = COMM_RECEIVING;
						}
				}
				//会有其他非正常数据发过来！！！组网成功命令前会多发一个0x03过来！--原因找到--带仿真器调试时会出现。
				if(commStatus2 == COMM_RECEIVING){
						if(ptrRcv2>=frameLen2-1)//zhh bug：frameLen1 -> frameLen2
						{
								ptrRcv2=0;
								commStatus2=COMM_FRAME_READY;  
						}else{
								ptrRcv2++;
						}
				}		
		}		
} 



void USART1_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif

		static u8 ptrRcv1 = 0;//接收缓冲区指针，不大于最大字节数：USART_REC_LEN = 10
	
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
		{			
				commRcvBuff1[ptrRcv1] =USART_ReceiveData(USART1);	//读取接收到的数据
				
				if(ptrRcv1==0){
					  commStatus1 = COMM_RECEIVING;
						switch(commRcvBuff1[0]){
							case COMM_SHAKEHAND:case COMM_READ_ID:case COMM_SENSOR_STATUS: case  COMM_BT_RESTORE:    //帧长为3字节
								case COMM_BIND_BEGIN:case COMM_BIND_CHECK:case COMM_UNBIND_SENSOR:case COMM_READ_ANGLE:	//帧长为3字节
										 frameLen1 = 3;		break;
								case COMM_READ_PRESSURE:
									   frameLen1 = 4;	  break;
								case COMM_SET_ID:	//帧长为10字节
									   frameLen1 = 10;	break;			
								case COMM_BT_TRANSPORT:					//zhh bug 未加透传指令码的处理
										 frameLen1 = 15;	break;		//帧长暂定为15字节，收到下一字节才能确定帧长度					 						
								default: 			//如果命令码不对，清除正在接收标志，重置帧长度
									   frameLen1 = 0; 
								     commStatus1 = COMM_IDLE;	
										 break;
						}									
				}				
				
				if(commStatus1 == COMM_RECEIVING){
						if(commRcvBuff1[0]==COMM_BT_TRANSPORT && ptrRcv1==1){//在透传指令下，第二字节为帧长
									frameLen1 = commRcvBuff1[1] + 3;							 //zhh bug 未加透传指令码的处理
						}
						if(ptrRcv1>=frameLen1-1)
						{								
								ptrRcv1 = 0;
								commStatus1 = COMM_FRAME_READY;								
						}else{
								ptrRcv1++;
						}
				}		
		}	
}


#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif

#endif	

