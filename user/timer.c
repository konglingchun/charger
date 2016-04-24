#include <stdio.h>

#include "stm32f10x.h"
#include "serial.h"
#include "timer.h"

system_timer_t uart1_time_trigger;
system_timer_t uart2_time_trigger;
system_timer_t uart3_time_trigger;

void timer2_init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM2��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx					 
}

int system_timer_init(
	system_timer_t *time,
	u16 day,
	u16 hour,
	u16 minute,
	u16 second,
	u16 msec)
{
	if(time != NULL)
	{
		if((day <= 356)
			&&(hour < 24)
			&&(minute < 60)
			&&(second < 60)
			&&(msec < 1000))
		{
			time->day = day;
			time->hour = hour;
			time->minute = minute;
			time->second = second;
			time->msec = msec;
			time->day_reload = day;
			time->hour_reload = hour;
			time->minute_reload = minute;
			time->second_reload = second;
			time->msec_reload = msec;
			time->start = TIMER_OFF;
			time->time_out = TIME_OUT_FALSE;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

int system_timer_reload(system_timer_t *time)
{
	if(time != NULL)
	{
		time->day = time->day_reload;
		time->hour = time->hour_reload;
		time->minute = time->minute_reload;
		time->second = time->second_reload;
		time->msec = time->msec_reload;
		time->start = TIMER_ON;
		time->time_out = TIME_OUT_FALSE;
		return 0;
	}
	return -1;
}

int system_timer_start(system_timer_t *time)
{
	if(time != NULL)
	{
		time->start = TIMER_ON;
		time->time_out = TIME_OUT_FALSE;
		return 0;
	}
	return -1;
}

int system_timer_stop(system_timer_t *time)
{
	if(time != NULL)
	{
		time->start = TIMER_OFF;
		time->time_out = TIME_OUT_FALSE;
		return 0;
	}
	return -1;
}

int is_system_timer_timer_on(system_timer_t *time)
{
	if(time != NULL)
	{
		if(time->start== TIMER_ON)//timer on
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

int is_system_timer_timeout(system_timer_t *time)
{
	if(time != NULL)
	{
		if(time->time_out== TIME_OUT_TRUE)//time out
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

int system_timer_clear_timeout(system_timer_t *time)
{
	if(time != NULL)
	{
		time->time_out = TIME_OUT_FALSE;
		return 0;
	}
	return -1;
}

int system_timer_tick(system_timer_t *time)
{
	if(time != NULL)
	{
		if((time->time_out == TIME_OUT_FALSE)
			&&(time->start == TIMER_ON))
		{
			if(time->msec > 0)
			{
				time->msec--;
			}
			else
			{
				time->msec= 999;
				if(time->second > 0)
				{
					time->second--;
				}
				else
				{
					time->second = 59;
					if(time->minute > 0)
					{
						time->minute--;
					}
					else
					{
						time->minute = 59;
						if(time->hour > 0)
						{
							time->hour--;
						}
						else
						{
							time->hour = 23;
							if(time->day > 0)
							{
								time->day--;
							}
							else
							{
								time->day = 0;
								time->hour = 0;
								time->minute = 0;
								time->second = 0;
								time->msec = 0;
								time->start= TIMER_OFF;
								time->time_out= TIME_OUT_TRUE;
							}
						}
					}
				}
			}
		}
		return 0;
	}
	return -1;
}

int system_timer_test(system_timer_t *time)
{
	if(time != NULL)
	{
		if(is_system_timer_timeout(time)==0 && is_system_timer_timer_on(time) == 1)
		{
			printf("day = %d, hour = %d, minute = %d, second = %d, msec = %d\r\n",
				time->day,
				time->hour,
				time->minute,
				time->second,
				time->msec);
		}
		return 0;
	}
	return -1;
}

void TIM2_IRQHandler(void)   //TIM2�ж�
{
	system_timer_tick(&uart1_time_trigger);
	if(uart1_data.status == UART_CACHE_LOAD)
	{
		if(is_system_timer_timeout(&uart1_time_trigger))
		{
			//printf("uart 1 time out\r\n");
			uart1_data.receive[uart1_data.length] = '\0';
			uart1_data.status = UART_CACHE_FULL;
			system_timer_stop(&uart1_time_trigger);
		}
	}
	//system_timer_test(&uart1_time_trigger);
	
	system_timer_tick(&uart2_time_trigger);
	if(uart2_data.status == UART_CACHE_LOAD)
	{
		if(is_system_timer_timeout(&uart2_time_trigger))
		{
			//printf("uart 2 time out\r\n");
			uart2_data.receive[uart2_data.length] = '\0';
			uart2_data.status = UART_CACHE_FULL;
			system_timer_stop(&uart2_time_trigger);
		}
	}
	//system_timer_test(&uart2_time_trigger);

	system_timer_tick(&uart3_time_trigger);
	if(uart3_data.status == UART_CACHE_LOAD)
	{
		if(is_system_timer_timeout(&uart3_time_trigger))
		{
			//printf("uart 3 time out\r\n");
			uart3_data.receive[uart3_data.length] = '\0';
			uart3_data.status = UART_CACHE_FULL;
			system_timer_stop(&uart3_time_trigger);
		}
	}
	//system_timer_test(&uart3_time_trigger);
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־ 
}

