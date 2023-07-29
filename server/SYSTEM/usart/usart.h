#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN 100  	//定义最大接收字节数 200
#define BUFF_LEN_MAX1 100		//串口1最大帧长（字节）
#define BUFF_LEN_MAX2 100		//串口2最大帧长（字节）
#define EN_USART1_RX 	1			//使能（1）/禁止（0）串口1接收

//通信命令码定义
#define COMM_SHAKEHAND 0x60  //握手命令
#define COMM_SET_ID 	 0x61	 //设置设备ID
#define COMM_READ_ID 	 0x62	 //读设备ID
#define COMM_SENSOR_STATUS 	 0x63	 	//读传感器地址绑定状态
#define COMM_BIND_BEGIN 	 0x64	 		//启动绑定传感器过程
#define COMM_BIND_CHECK	 0x65	 			//查询绑定过程
#define COMM_UNBIND_SENSOR	 0x66	 	//绑定传感器
#define COMM_READ_ANGLE 	 0x67	 		//读取角度
#define COMM_BT_RESTORE	 	 0x68	 		//蓝牙模块恢复出厂配置
#define COMM_READ_PRESSURE	 	0x69	//读取压力
#define COMM_BT_TRANSPORT 	 0x6A	 	//主蓝牙模块的透传命令：就是在主机上加上本命令码与校验和

//通信错误码定义
#define COMM_NO_ERR 0     			//无错误返回参数
#define COMM_CHECKSUM_ERR 3     //校验和错误
#define COMM_SHAKEHAND_ERR 1    //握手命令错误
#define COMM_FLASH_ERR 1    		//flash发生错误

#define COMM_POSITION_ERR 1    		//给的传感器位置错误
#define COMM_OTHER_BOUND_ERR 4    //已有绑定的传感器
#define COMM_NO_BOUND_ERR 5    		//没有绑定传感器，无需解绑

//通信状态定义
#define COMM_IDLE 0    					//0--空闲
#define COMM_RECEIVING 1    		//1--接收通信帧中
#define COMM_FRAME_READY 2    	//2--帧已接收，待处理

//**************蓝牙命令和消息********************
//控制消息类型 （msgCtlType）
#define CONTROL_MSG_CONFIG		0xC0		//配置消息
#define CONTROL_MSG_SIG				0xC1		//SIG消息
#define CONTROL_MSG_TRANS			0xC2		//透传消息
//收到的消息类型（msgRcvType）							
#define RESPONSE_MSG_CONFIG		0x40		//配置消息
#define RESPONSE_MSG_SIG			0x41		//SIG消息
#define RESPONSE_MSG_TRANS		0x42		//透传消息			
#define RESPONSE_MSG_ERR			0x43		//发生错误	

//组网进程状态 (netAddStatus)
#define NETADD_IDLE						  0		//空闲状态
#define NET_ADDING							1		//正在组网
#define NETADD_SUCCESS					2		//组网成功
#define NETADD_FAILURE					3		//组网失败（错误码保存在btErrCode)
#define NETADD_FLASH_FAILURE		4		//保存FLASH时发生错误（错误码保存在btErrCode)

//操作码（msgOpCode）
#define NETKEY_SET		  0x00		//配置网络密匙（只用于主模块）
#define NETKEY_RD		  	0x01		//读网络密匙（主）
#define APPKEY_SET		  0x02		//配置应用密匙（主）
#define APPKEY_RD		  	0x03		//读应用密匙（主）
#define NEXT_ADDR_SET  	0x04		//设置下次的组网地址（主）
#define NEXT_ADDR_RD	  0x05		//读下次的组网地址（主）
#define FILTER_SET		  0x06		//设置组网过滤器（主）
#define FILTER_DEL		  0x07		//删除组网过滤器（主）
#define FILTER_RD		  	0x08		//读组网过滤器（主）
#define NETWORK_ADDIN 	0x09		//启动组网:设备入网（主）
#define BAUDRATE_SET		0x0E		//设置波特率(主&从)
#define BAUDRATE_RD		  0x0F		//读波特率（主&从）
#define ADDRESS_RD		  0x0B		//获取网络地址（只用于从模块）
#define MAC_ADDR_RD		  0x11		//读MAC地址
#define POWER_SET		  	0x12		//设置功率
#define POWER_RD		  	0x13		//读功率
#define DEVICE_RESET		0x14		//重启蓝牙模块
#define FACTORY_RESTORE	0x15		//恢复出厂设置
#define VERSION_RD		  0x16		//读版本号
#define DEVICE_SLEEP		0x17		//进入睡眠（只用于从模块）
#define SLEEP_STATUS_RD	0x18		//读睡眠状态（只用于从模块）

//蓝牙错误码(btErrCode)
#define BT_INVALID_PARAM_ERR		  0xFF		//无效参数
#define BT_BUSY_ERR		  					0xFE		//操作忙
#define BT_INVALID_CMD_ERR		  	0xFD		//无效命令
#define BT_NO_NODE_ERR		  			0xFC		//无组网设备
#define BT_PARAM_GAIN_ERR		  		0xFB		//获取参数失败
#define BT_NETKEY_ERR		  				0xFA		//分配netkey失败
#define BT_APPKEY_TIMEOUT_ERR		  0xF9		//绑定appkey超时
#define BT_APPKEY_FAILURE_ERR		  0xF8		//绑定appkey失败
#define BT_NET_ADDING_ERR		  		0xF7		//上次组网还未结束
//续：本应用添加的错误码
#define BT_NET_ADDING_NULL	  		0xE0		//无组网过程
#define BT_NET_ADDING_ONGO	  		0xE1		//正在组网中
#define BT_NET_ADDING_FLASH_ERR		0xE2		//正在组网中

extern u8 suitMAC[8];
extern u8 datatemp[29];
struct inf
{
	u8 IP;
	u8 buwei;
	u8 state;
};

static struct inf message[7]; 

extern u8 bindinf[7];

//串口1变量
extern u8 commStatus1; 					//串口1通信状态
extern u8 commRcvBuff1[BUFF_LEN_MAX1];  		//串口1接收缓冲区
extern u8 commSendBuff1[BUFF_LEN_MAX1];  	//串口1发送缓冲区
extern u8 frameLen1;           	//串口1帧长
//串口2变量
extern u8 commStatus2; 					//串口2通信状态
extern u8 commRcvBuff2[BUFF_LEN_MAX2];  		//串口2接收缓冲区
extern u8 commSendBuff2[BUFF_LEN_MAX2];  	//串口2发送缓冲区
extern u8 frameLen2;           //串口2帧长

//如果想串口中断接收，请不要注释以下宏定义
void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void USART3_Init(u32 bound);

//******通信函数*********//
void SendToPC(u8 length ,u8 *s);						//发数据到上位机
u8 GetCheckSum(u8 inLen,u8 *inBuff);				//校验和计算
u8 CheckSumTest(u8 inLen,u8 *inBuff);				//校验和判断
void SendShortCmd(u8 inCmdCode,u8 inParam);	//发送短命令（3字节）到上位机：命令码+参数+校验和

void BTSendData(int length ,u8 *s);						//发数据到下位机--通过蓝牙模块
void BTSendCfg(u8 inOpCode);								//发送蓝牙命令--配置命令，只适用于3字节短指令
void BTSetNextAddr(u16 inAddr);							//发送蓝牙命令--下一个组网地址设置命令
void BTDelNode(u16 InNodeAddr);							//发送蓝牙命令--SIG命令：删除节点命令
void BTCmdToSensor(u8 inCmdCode,u8 inPosition,u16 InNodeAddr); 	//发送蓝牙命令到传感器--通过透传消息命令（注：蓝牙模块的透传命令--与上位机的透传不同）

void FrameParse1(void);	//串口1通信解包
void FrameParse2(void);	//串口2通信解包

#endif