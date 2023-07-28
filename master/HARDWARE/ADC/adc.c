 #include "adc.h"
 #include "delay.h"
 
#include "stm32f10x.h"
#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t ADC_ConvertedValue[Channel_Num];

static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	/* Configure PA4-PA7��PB0  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//����ʱ������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//ģ����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//����ʱ������������
}

static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* Enable ADC1 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);	//DMA1��ͨ��1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//����Ϊ����Դ
	DMA_InitStructure.DMA_BufferSize = Channel_Num;//������DMAҪ����������ܴ�С��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//�������ݵ�λ������16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//�������ݵ�λ������16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ADCģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 //ɨ��ģʽ���ڶ�ͨ���ɼ�
	//ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//����������ת��ģʽ��������������ADCת��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ������ͣ�ؽ���ADCת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 5;	 	//Ҫת����ͨ����Ŀ1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M 
	/*����ADC1��ͨ��4-7Ϊ239.5���������ڣ�����Ϊ1 */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��4,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��5,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��6,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��7,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 5, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��8,����ʱ��Ϊ239.5����
	
	
	ADC_DMACmd(ADC1, ENABLE);	// Enable ADC1 DMA
	ADC_Cmd(ADC1, ENABLE);	// Enable ADC1
	
	ADC_ResetCalibration(ADC1);	//��λУ׼�Ĵ���  
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ�У׼�Ĵ�����λ���
	ADC_StartCalibration(ADC1);	//ADCУ׼ 
	while(ADC_GetCalibrationStatus(ADC1));	//�ȴ�У׼���
	 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// ����û�в����ⲿ����������ʹ���������ADCת��
}



void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}