#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN 100  	//�����������ֽ��� 200
#define BUFF_LEN_MAX1 100		//����1���֡�����ֽڣ�
#define BUFF_LEN_MAX2 100		//����2���֡�����ֽڣ�
#define EN_USART1_RX 	1			//ʹ�ܣ�1��/��ֹ��0������1����

//ͨ�������붨��
#define COMM_SHAKEHAND 0x60  //��������
#define COMM_SET_ID 	 0x61	 //�����豸ID
#define COMM_READ_ID 	 0x62	 //���豸ID
#define COMM_SENSOR_STATUS 	 0x63	 	//����������ַ��״̬
#define COMM_BIND_BEGIN 	 0x64	 		//�����󶨴���������
#define COMM_BIND_CHECK	 0x65	 			//��ѯ�󶨹���
#define COMM_UNBIND_SENSOR	 0x66	 	//�󶨴�����
#define COMM_READ_ANGLE 	 0x67	 		//��ȡ�Ƕ�
#define COMM_BT_RESTORE	 	 0x68	 		//����ģ��ָ���������
#define COMM_READ_PRESSURE	 	0x69	//��ȡѹ��
#define COMM_BT_TRANSPORT 	 0x6A	 	//������ģ���͸����������������ϼ��ϱ���������У���

//ͨ�Ŵ����붨��
#define COMM_NO_ERR 0     			//�޴��󷵻ز���
#define COMM_CHECKSUM_ERR 3     //У��ʹ���
#define COMM_SHAKEHAND_ERR 1    //�����������
#define COMM_FLASH_ERR 1    		//flash��������

#define COMM_POSITION_ERR 1    		//���Ĵ�����λ�ô���
#define COMM_OTHER_BOUND_ERR 4    //���а󶨵Ĵ�����
#define COMM_NO_BOUND_ERR 5    		//û�а󶨴�������������

//ͨ��״̬����
#define COMM_IDLE 0    					//0--����
#define COMM_RECEIVING 1    		//1--����ͨ��֡��
#define COMM_FRAME_READY 2    	//2--֡�ѽ��գ�������

//**************�����������Ϣ********************
//������Ϣ���� ��msgCtlType��
#define CONTROL_MSG_CONFIG		0xC0		//������Ϣ
#define CONTROL_MSG_SIG				0xC1		//SIG��Ϣ
#define CONTROL_MSG_TRANS			0xC2		//͸����Ϣ
//�յ�����Ϣ���ͣ�msgRcvType��							
#define RESPONSE_MSG_CONFIG		0x40		//������Ϣ
#define RESPONSE_MSG_SIG			0x41		//SIG��Ϣ
#define RESPONSE_MSG_TRANS		0x42		//͸����Ϣ			
#define RESPONSE_MSG_ERR			0x43		//��������	

//��������״̬ (netAddStatus)
#define NETADD_IDLE						  0		//����״̬
#define NET_ADDING							1		//��������
#define NETADD_SUCCESS					2		//�����ɹ�
#define NETADD_FAILURE					3		//����ʧ�ܣ������뱣����btErrCode)
#define NETADD_FLASH_FAILURE		4		//����FLASHʱ�������󣨴����뱣����btErrCode)

//�����루msgOpCode��
#define NETKEY_SET		  0x00		//���������ܳף�ֻ������ģ�飩
#define NETKEY_RD		  	0x01		//�������ܳף�����
#define APPKEY_SET		  0x02		//����Ӧ���ܳף�����
#define APPKEY_RD		  	0x03		//��Ӧ���ܳף�����
#define NEXT_ADDR_SET  	0x04		//�����´ε�������ַ������
#define NEXT_ADDR_RD	  0x05		//���´ε�������ַ������
#define FILTER_SET		  0x06		//��������������������
#define FILTER_DEL		  0x07		//ɾ������������������
#define FILTER_RD		  	0x08		//������������������
#define NETWORK_ADDIN 	0x09		//��������:�豸����������
#define BAUDRATE_SET		0x0E		//���ò�����(��&��)
#define BAUDRATE_RD		  0x0F		//�������ʣ���&�ӣ�
#define ADDRESS_RD		  0x0B		//��ȡ�����ַ��ֻ���ڴ�ģ�飩
#define MAC_ADDR_RD		  0x11		//��MAC��ַ
#define POWER_SET		  	0x12		//���ù���
#define POWER_RD		  	0x13		//������
#define DEVICE_RESET		0x14		//��������ģ��
#define FACTORY_RESTORE	0x15		//�ָ���������
#define VERSION_RD		  0x16		//���汾��
#define DEVICE_SLEEP		0x17		//����˯�ߣ�ֻ���ڴ�ģ�飩
#define SLEEP_STATUS_RD	0x18		//��˯��״̬��ֻ���ڴ�ģ�飩

//����������(btErrCode)
#define BT_INVALID_PARAM_ERR		  0xFF		//��Ч����
#define BT_BUSY_ERR		  					0xFE		//����æ
#define BT_INVALID_CMD_ERR		  	0xFD		//��Ч����
#define BT_NO_NODE_ERR		  			0xFC		//�������豸
#define BT_PARAM_GAIN_ERR		  		0xFB		//��ȡ����ʧ��
#define BT_NETKEY_ERR		  				0xFA		//����netkeyʧ��
#define BT_APPKEY_TIMEOUT_ERR		  0xF9		//��appkey��ʱ
#define BT_APPKEY_FAILURE_ERR		  0xF8		//��appkeyʧ��
#define BT_NET_ADDING_ERR		  		0xF7		//�ϴ�������δ����
//������Ӧ����ӵĴ�����
#define BT_NET_ADDING_NULL	  		0xE0		//����������
#define BT_NET_ADDING_ONGO	  		0xE1		//����������
#define BT_NET_ADDING_FLASH_ERR		0xE2		//����������

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

//����1����
extern u8 commStatus1; 					//����1ͨ��״̬
extern u8 commRcvBuff1[BUFF_LEN_MAX1];  		//����1���ջ�����
extern u8 commSendBuff1[BUFF_LEN_MAX1];  	//����1���ͻ�����
extern u8 frameLen1;           	//����1֡��
//����2����
extern u8 commStatus2; 					//����2ͨ��״̬
extern u8 commRcvBuff2[BUFF_LEN_MAX2];  		//����2���ջ�����
extern u8 commSendBuff2[BUFF_LEN_MAX2];  	//����2���ͻ�����
extern u8 frameLen2;           //����2֡��

//����봮���жϽ��գ��벻Ҫע�����º궨��
void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void USART3_Init(u32 bound);

//******ͨ�ź���*********//
void SendToPC(u8 length ,u8 *s);						//�����ݵ���λ��
u8 GetCheckSum(u8 inLen,u8 *inBuff);				//У��ͼ���
u8 CheckSumTest(u8 inLen,u8 *inBuff);				//У����ж�
void SendShortCmd(u8 inCmdCode,u8 inParam);	//���Ͷ����3�ֽڣ�����λ����������+����+У���

void BTSendData(int length ,u8 *s);						//�����ݵ���λ��--ͨ������ģ��
void BTSendCfg(u8 inOpCode);								//������������--�������ֻ������3�ֽڶ�ָ��
void BTSetNextAddr(u16 inAddr);							//������������--��һ��������ַ��������
void BTDelNode(u16 InNodeAddr);							//������������--SIG���ɾ���ڵ�����
void BTCmdToSensor(u8 inCmdCode,u8 inPosition,u16 InNodeAddr); 	//�����������������--ͨ��͸����Ϣ���ע������ģ���͸������--����λ����͸����ͬ��

void FrameParse1(void);	//����1ͨ�Ž��
void FrameParse2(void);	//����2ͨ�Ž��

#endif