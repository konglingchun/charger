#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <RTL.h>

#include "stm32f10x.h"
#include "sim900a.h"
#include "serial.h"
#include "timer.h"
#include "charger.h"

//#define DEBUG_MSG

os_mbx_declare(sim900a_receive, 20);

//ÀýÈç°´ÏÂ Ctrl+z¼ü£¬»á²úÉú¿ØÖÆÂë
char get_ctrl_code(char code)
{
	//¿ØÖÆÂëÎªASCIIÂëÎ»Óë0x3f
	return code&0x3f;
}

void sim900_power_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//±?Dè?è??ê±?ó
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void sim900_net_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void sim900a_power_on(void)
{
	int i;
	int power_status = 0;
	int confirm_times = 0;
	
	/* detect if power is on */
	for(i=0;i<300;i++)
	{
		power_status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
		//printf("power_status = %d\r\n", power_status);
		if(power_status == 1)
		{
			printf("power has been turned on, debug:%d\r\n", i);
			confirm_times++;
			if(confirm_times >= 2)
			{
				return;//power on
			}
		}
		os_dly_wait(2);
	}
	/* power on */
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	os_dly_wait(100);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
	os_dly_wait(300);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	/* wait for sim900a power on */
	do
	{
		power_status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
	}while(power_status != 1);
	printf("power has been turned on\r\n");
	charger.sim900a_power = ON;
}

void sim900a_power_off(void)
{
	int i;
	int power_status= 0;

	charger.sim900a_power = OFF;
	/* detect if power is on */
	for(i=0;i<100;i++)
	{
		power_status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
		if(power_status == 1)
		{
			/* power off */
			GPIO_SetBits(GPIOB, GPIO_Pin_4);
			os_dly_wait(400);
			GPIO_ResetBits(GPIOB, GPIO_Pin_4);
			os_dly_wait(400);
			printf("power has been turned off, debug:%d\r\n", i);
			return;
		}
		os_dly_wait(8);
	}
	printf("power has been turned off\r\n");
}

void sim900a_init(void)
{
	sim900_power_pin_init();
	sim900_net_pin_init();
	sim900a_power_on();
	os_mbx_init(&sim900a_receive, sizeof(sim900a_receive));
	uart3_init();
	system_timer_init(&uart3_time_trigger,0,0,0,0,5);
	uart3_nvic_init();
}

void sim900a_command_result(char *command, int ret)
{
	if(SIM900A_COMMAND_OK == ret)
	{
		printf("%s, OK\r\n", command);
	}
	else if(SIM900A_COMMAND_ERROR == ret)
	{
		printf("%s, ERROR\r\n", command);
	}
}

int sim900a_send(char *command, int cmd_size, int retry_times, U16 timeout)
{
	int i = 0;
	char *msg = NULL;
	char result[10] = "";
	char end;

	uart3_send_str(command, cmd_size);
	for(i=0;i<retry_times;i++)
	{	
		if(os_mbx_wait(&sim900a_receive, (void **)&msg, timeout) == OS_R_TMO)
		{
			printf ("Wait message timeout times = %d!\r\n", i);
		}
		else
		{
			/* process message here */
#ifdef DEBUG_MSG

			printf("\r\n======================================\r\n");
			printf("msg = [%s]\r\n", msg);
			printf("\r\n======================================\r\n");
			
#endif
			end = get_ctrl_code('Z');
			if(strstr(command, "AT+CIPSEND"))
			{
				sscanf(msg, "%*[^\n]%s", result);
				//printf("result1 = [%s]\r\n", result);
				if(strcmp(result, ">") == 0)
				{
					free(msg);
					return SIM900A_COMMAND_OK;
				}
				else
				{
					free(msg);
					return SIM900A_COMMAND_ERROR;
				}
			}
			else if(strstr(command, "AT+CIPCLOSE")
				||strstr(command, "AT+CIPSHUT")
				||(command[0] == end))
			{
				sscanf(msg, "%*[^\n]%*s%s", result);
				//printf("result2 = [%s]\r\n", result);
				if(strcmp(result, "OK") == 0)
				{
					free(msg);
					return SIM900A_COMMAND_OK;
				}
				else
				{
					free(msg);
					return SIM900A_COMMAND_ERROR;
				}
			}
			else if(strstr(command, "AT+CBC"))
			{
				sscanf(msg, "%*[^\n]%*s%*[^\n]%s", result);
				//printf("result4 = [%s]\r\n", result);
				if(strcmp(result, "OK") == 0)
				{
					free(msg);
					return SIM900A_COMMAND_OK;
				}
				else
				{
					free(msg);
					return SIM900A_COMMAND_ERROR;
				}
			}
			else
			{
				sscanf(msg, "%*[^\n]%s", result);
				//printf("result3 = [%s]\r\n", result);
				if(strcmp(result, "OK") == 0)
				{
					free(msg);
					return SIM900A_COMMAND_OK;
				}
				else
				{
					free(msg);
					return SIM900A_COMMAND_ERROR;
				}
			}
		 }
	}
	return SIM900A_COMMAND_ERROR;
}

int sim900a_querry(char *command, int cmd_size, char *querry, int querry_size, int retry_times, U16 timeout)
{
	int i = 0;
	char *msg = NULL;
	char querry_temp[30] = "";
	int length = 0;
	char result[10] = "";
	char *ptr = NULL;
	char ch;

	uart3_send_str(command, cmd_size);
	for(i=0;i<retry_times;i++)
	{	
		if(os_mbx_wait(&sim900a_receive, (void **)&msg, timeout) == OS_R_TMO)
		{
			printf ("Wait message timeout times = %d!\r\n", i);
		}
		else
		{
			/* process message here */
#ifdef DEBUG_MSG
			printf("\r\n======================================\r\n");
			printf("msg = [%s]\r\n", msg);
			printf("\r\n======================================\r\n");
			
#endif
			sscanf(msg, "%*[^\n]%s%*[^\n]%s", querry_temp, result);
			ptr = strstr(msg, querry_temp);
			memset(querry_temp, 0, sizeof(querry_temp));
			while(1)
			{	
				ch = *(ptr+length);
				//printf("ch = %#x,%c\r\n", ch, ch);
				if(ch != '\r')
				{	
					querry_temp[length] = ch;
					length++;
				}	
				else
				{	
					break;
				}	
			}
			if(length > querry_size)
			{
				free(msg);
				return SIM900A_COMMAND_ERROR;
			}
			else
			{
				memcpy(querry, querry_temp, length);
			}
			//printf("querry_temp = [%s], result = [%s]\r\n", querry_temp, result);
			if(strcmp(result, "OK") == 0)
			{
				free(msg);
				return SIM900A_COMMAND_OK;
			}
			else
			{
				free(msg);
				return SIM900A_COMMAND_ERROR;
			}
		 }
	}
	return SIM900A_COMMAND_ERROR;
}

#if 0


#endif

