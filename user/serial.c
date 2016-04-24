#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "serial.h"
#include "timer.h"

uart_t uart1_data;
uart_t uart2_data;
uart_t uart3_data;

int put_char(int ch)
{
	USART_SendData(DEBUG_USART, (u8)ch);
	while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TC) == RESET);
	return ch;
}

int get_char(void)
{
	while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);
	return (u8)USART_ReceiveData(DEBUG_USART);
}

/* 重定义fputc以便支持printf */
int fputc(int ch, FILE *f)
{
	return put_char(ch);
}

/* 重定义fgetc以便支持scanf */
int fgetc(FILE *f)
{
	char ch;

	ch = get_char();
	return put_char(ch);
}

void uart1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	/* remap USART1 Tx (PB.06) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10) as input floating */
	/* remap USART1 Rx (PB.07) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 configuration ------------------------------------------------------*/
	// USART1 configured as follow:
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USART1 */
	USART_Init(USART1, &USART_InitStructure);

	/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
	 USART1 transmit data register is empty */  
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	 USART1 receive data register is not empty */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);
}

void uart2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE);

	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Rx (PA.03) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART2 configuration ------------------------------------------------------*/
	// USART2 configured as follow:
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USART2 */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
	 USART1 transmit data register is empty */  
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	 USART2 receive data register is not empty */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* Enable USART2 */
	USART_Cmd(USART2, ENABLE);
}

void uart3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//USART3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);

	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* USART3 configuration ------------------------------------------------------*/
	// USART3 configured as follow:
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USART2 */
	USART_Init(USART3, &USART_InitStructure);

	/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
	 USART1 transmit data register is empty */  
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	 USART2 receive data register is not empty */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	/* Enable USART2 */
	USART_Cmd(USART3, ENABLE);
}

void uart_data_init(uart_t *data)
{
	memset(data, 0, sizeof(uart_t));
	data->status = UART_CACHE_EMPTY;
}

void uart1_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		   

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//enable
	NVIC_Init(&NVIC_InitStructure);
	uart_data_init(&uart1_data);
}

void uart2_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		   

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//enable
	NVIC_Init(&NVIC_InitStructure);
	uart_data_init(&uart2_data);
}

void uart3_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		   

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//enable
	NVIC_Init(&NVIC_InitStructure);
	uart_data_init(&uart3_data);
}

void uart1_send_str(char *str, u16 length)
{
	u16 i;
	
	USART_ClearFlag(USART1, USART_FLAG_TC);

	for(i=0;i<length;i++)
	{
		USART_SendData(USART1, (u8)str[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}

void uart2_send_str(char *str, u16 length)
{
	u16 i;
	
	USART_ClearFlag(USART2, USART_FLAG_TC);

	for(i=0;i<length;i++)
	{
		USART_SendData(USART2, (u8)str[i]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	}
}

void uart3_send_str(char *str, u16 length)
{
	u16 i;
	
	USART_ClearFlag(USART3, USART_FLAG_TC);

	for(i=0;i<length;i++)
	{
		#if 0
		if((str[i]>='a')
			&&(str[i]<='z'))//Lower case to upper case
		{
			str[i] = str[i] - 'a' + 'A';
		}
		#endif
		USART_SendData(USART3, (u8)str[i]);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	}
	USART_SendData(USART3, '\r');
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3, '\n');
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

int read_uart_data(uart_t *data, char *buffer, int buffer_size)
{
	int ret = 0;
	
	if(data->status == UART_CACHE_FULL)
	{
		if(data->length <= buffer_size)
		{
			memset(buffer, 0, buffer_size);
			memcpy(buffer, data->receive, data->length);
			ret = data->length;
		}
		else
		{
			ret = -1;
		}
		uart_data_init(data);
	}
	return ret;
}

void USART1_IRQHandler(void)
{
	u8 ch;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		system_timer_reload(&uart1_time_trigger);
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		ch = USART_ReceiveData(USART1);
		put_char(ch);
		if(uart1_data.status == UART_CACHE_EMPTY)
		{
			uart1_data.status = UART_CACHE_LOAD;
			uart1_data.length = 0;
		}
		if(uart1_data.status == UART_CACHE_LOAD)
		{
			uart1_data.receive[uart1_data.length] = ch;
			uart1_data.length++;
			if(uart1_data.length >= UART_BUFFER_LEN)
			{
				uart1_data.receive[uart1_data.length] = '\0';
				uart1_data.status = UART_CACHE_FULL;
				system_timer_stop(&uart1_time_trigger);
			}
			else if(ch == '\n'
				||ch == '\r')
			{
				uart1_data.length--;
				uart1_data.receive[uart1_data.length] = '\0';
				uart1_data.status = UART_CACHE_FULL;
				system_timer_stop(&uart1_time_trigger);
			}
		}
	}
}

void USART2_IRQHandler(void)
{
	u8 ch;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		system_timer_reload(&uart2_time_trigger);
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		ch = USART_ReceiveData(USART2);
		if(uart2_data.status == UART_CACHE_EMPTY)
		{
			uart2_data.status = UART_CACHE_LOAD;
			uart2_data.length = 0;
		}
		if(uart2_data.status == UART_CACHE_LOAD)
		{
			uart2_data.receive[uart2_data.length] = ch;
			uart2_data.length++;
			if(uart2_data.length >= UART_BUFFER_LEN)
			{
				uart2_data.receive[uart2_data.length] = '\0';
				uart2_data.status = UART_CACHE_FULL;
				system_timer_stop(&uart2_time_trigger);
			}
			else if(ch == '\n'
				||ch == '\r')
			{
				uart2_data.length--;
				uart2_data.receive[uart2_data.length] = '\0';
				uart2_data.status = UART_CACHE_FULL;
				system_timer_stop(&uart2_time_trigger);
			}
		}
	}
}

void USART3_IRQHandler(void)
{
	u8 ch;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		system_timer_reload(&uart3_time_trigger);
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		ch = USART_ReceiveData(USART3);
		if(uart3_data.status == UART_CACHE_EMPTY)
		{
			uart3_data.status = UART_CACHE_LOAD;
			uart3_data.length = 0;
		}
		if(uart3_data.status == UART_CACHE_LOAD)
		{
			uart3_data.receive[uart3_data.length] = ch;
			uart3_data.length++;
			if(uart3_data.length >= UART_BUFFER_LEN)
			{
				uart3_data.receive[uart3_data.length] = '\0';
				uart3_data.status = UART_CACHE_FULL;
				system_timer_stop(&uart3_time_trigger);
			}
		}
	}
}

