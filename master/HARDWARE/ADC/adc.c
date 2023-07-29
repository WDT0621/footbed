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
	
	/* Configure PA4-PA7、PB0  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//输入时不用设置速率
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//输入时不用设置速率
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
	DMA_DeInit(DMA1_Channel1);	//DMA1的通道1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//外设为数据源
	DMA_InitStructure.DMA_BufferSize = Channel_Num;//保存了DMA要传输的数据总大小，
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据单位，半字16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//内设数据单位，半字16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//禁止内存到内存的传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 //扫描模式用于多通道采集
	//ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//不开启连续转换模式，即不连续进行ADC转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 5;	 	//要转换的通道数目1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M 
	/*配置ADC1的通道4-7为239.5个采样周期，序列为1 */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道4,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道5,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道6,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道7,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 5, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道8,采样时间为239.5周期
	
	
	ADC_DMACmd(ADC1, ENABLE);	// Enable ADC1 DMA
	ADC_Cmd(ADC1, ENABLE);	// Enable ADC1
	
	ADC_ResetCalibration(ADC1);	//复位校准寄存器  
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待校准寄存器复位完成
	ADC_StartCalibration(ADC1);	//ADC校准 
	while(ADC_GetCalibrationStatus(ADC1));	//等待校准完成
	 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// 由于没有采用外部触发，所以使用软件触发ADC转换
}



void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}