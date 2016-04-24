#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include "misc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "adc_driver.h"


uint16_t adc_buf[ADC_CHANNELS];
#define ADC_CYC_TIME	ADC_SampleTime_239Cycles5	
#define ADC1_DR_Address    ((u32)0x4001244C)

#define		DMA_INTERRUPT_USE	1
uint8_t ADC_complete;

adc_param_struct adc_param;

void ADC1_DMA_Start(void);

void InitADCValue(void)
{
	ADC_complete = 0;
}

static void INIT_ADC_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

static void ConfigADC(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNELS;
	ADC_Init(ADC1, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_TIME);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_TIME);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_TIME);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_TIME);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 5, ADC_TIME);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 6, ADC_TIME);	


}

static void ADC_DMA_Init(void)
{
#if	DMA_INTERRUPT_USE	
	NVIC_InitTypeDef NVIC_InitStructure;
		   
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;			     	//setting channel 2 of dam 1 for uart tx
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                             //开DMA时钟
	ADC_DMACmd(ADC1,ENABLE);
	
}

uint16_t GetMaxValue(uint8_t index,uint16_t *dst,uint8_t num)
{
	uint16_t temp[256];
	uint8_t i,j;
	uint16_t temp1;
	memcpy(temp,dst,num*2);
	for(i=0;i<(num-1);i++)
	{
		
		for(j=(i+1);j<num;j++)
		{
			if(temp[i]<temp[j])
			{
				temp1 = temp[i];
				temp[i] = temp[j];
				temp[j] = temp1;
				
			}
		}
	}
	return temp[index];
}

#if	DMA_INTERRUPT_USE	

void DMA1_Channel1_IRQHandler(void)
{
	uint8_t i;
	if(DMA_GetITStatus(DMA1_IT_TC1)) //
	{
		DMA_ClearITPendingBit(DMA1_IT_GL1);    //clear flag
		for(i=0;i<ADC_CHANNELS;i++)
		{
			adc_param.tim_adc_buf[i][adc_param.index] = adc_buf[i];
		}
		adc_param.index++;	
		
		if(adc_param.index >= ADC_TIM_BUF)
		{
			
			for(i=0;i<ADC_CHANNELS;i++)
			{
				//获取40次采样中的第二大的值
				adc_param.tim_200ms_buf[i][adc_param.tim_200ms_index]= GetMaxValue(1,adc_param.tim_adc_buf[i],ADC_TIM_BUF);
			}
			adc_param.tim_200ms_index++;
			if(adc_param.tim_200ms_index >= ADC_200MS_BUF)
			{
				for(i=0;i<ADC_CHANNELS;i++)
				{
					//获取10次最大值的平均值
					adc_param.adc_use_buf[i]= GetMaxValue(ADC_200MS_BUF/2,adc_param.tim_200ms_buf[i],ADC_200MS_BUF);
				}
				adc_param.tim_200ms_index = 0;
			}
			adc_param.index = 0;
			
		}
	}
}

#endif




void ADC1_DMA_Start(void)
{
	
	DMA_InitTypeDef DMA_InitStructure;
	/* DMA channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)adc_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_CHANNELS;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
#if	DMA_INTERRUPT_USE		
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
#else
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
#endif
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
#if	DMA_INTERRUPT_USE	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
#endif
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void timer3_init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}

void TIM3_IRQHandler(void)   //TIM2中断
{
	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) {
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);//clear flag
		
		
		ADC1_DMA_Start();//500us采集一次	
		
		

		

	}
}




void ADC1_Init(void)
{
	INIT_ADC_GPIO();
	ADC_DMA_Init();
	ConfigADC();
	timer3_init(500,(72-1));//500
	memset(&adc_param,0,sizeof(adc_param));
	ADC1_DMA_Start();	
}

void ADC1Stop(void)
{
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	ADC_Cmd(ADC1, DISABLE);
}

uint16_t GetADCValue(uint8_t channel_index)
{
	if(channel_index>=ADC_CHANNELS)
		return 0;
	return adc_param.adc_use_buf[channel_index];
}



