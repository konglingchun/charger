#ifndef		__ADC_DRIVER_H
#define		__ADC_DRIVER_H
#define		ADC_LENGTH			8
#define		ADC_CHANNELS		6
#define		ADC_TIME			ADC_SampleTime_28Cycles5//ADC_SampleTime_239Cycles5
#define		ADC_TIMES		1
#define		ADC_TIM_BUF		40
#define		ADC_200MS_BUF	10

typedef	struct ADC_PARAM_STRUCT_UNIT
{
	uint16_t index;
	uint16_t tim_adc_buf[ADC_CHANNELS][ADC_TIM_BUF];
	uint8_t tim_adc_index;

	uint16_t tim_200ms_buf[ADC_CHANNELS][ADC_200MS_BUF];
	uint8_t tim_200ms_index;

	uint16_t adc_use_buf[ADC_CHANNELS];	//外部使用的ADC值
}adc_param_struct;


void ADC1_Init(void);

uint16_t GetADCValue(uint8_t channel_index);



#endif
