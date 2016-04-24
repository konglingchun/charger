#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <RTL.h>

#include "stm32f10x.h"
#include "charger.h"
#include "serial.h"
#include "timer.h"
#include "sim900a.h"
#include "charger_command.h"

OS_TID tid_task_1;
OS_TID tid_task_2;

static U64 task_stack_1[2000/8];
static U64 task_stack_2[4000/8];

charger_t charger;

void board_debug_init(void)
{
	uart1_init();
	system_timer_init(&uart1_time_trigger,0,0,0,3,0);
	uart1_nvic_init();
	printf("\ncompile date:%s time: %s\r\n", __DATE__, __TIME__);
	printf("system start ...\r\n");
}

void board_timer_init(void)
{
	timer2_init(1000, 72);// 1 ms 中断一次
}

void print_chip_id(chip_id_t chip_id)
{
	int i;
	char buffer[25] = "";
	int offset = 0;

	for(i=0;i<12;i++)
	{
		snprintf(buffer+offset, sizeof(buffer)-offset, "%02x", chip_id.id[i]);
		offset += 2;
	}
	printf("chip id =[%s]\r\n", buffer);
}

void charger_data_init(void)
{
	u32 chip_id_addr = 0x1ffff7e8;
	
	charger.sim900a_power = OFF;
	charger.uart3_debug = OFF;
	charger.protocol = TCP;
	memset(charger.tel_num, 0, sizeof(charger.tel_num));
	charger.message_index = 0;
	charger.encrypt = NONE;
	charger.server_ip[3] = 58;
	charger.server_ip[2] = 215;
	charger.server_ip[1] = 235;
	charger.server_ip[0] = 62;
	charger.server_port = 61233;
	memcpy(&charger.chip_id.id, (void *)chip_id_addr, sizeof(charger.chip_id));
	print_chip_id(charger.chip_id);
}

void board_init(void)
{
	board_debug_init();
	charger_data_init();
	board_timer_init();
	sim900a_init();
}

__task void process_uart1(void)
{
	while(1)
	{
		int ret = 0;
		char command[256] = "";
		
		ret = read_uart_data(&uart1_data, command, sizeof(command));
		if(ret > 0)
		{
			//printf("uart1 receive[%d]=[%s]\r\n", ret, command);
			if(charger_command(command) < 0)
			{
				printf("unsupported command\r\n");
			}
		}
	}
}

void print_buffer(char *buffer, int length)
{
	int i;

	for(i=0;i<length;i++)
	{
		printf("\tbuffer[%d]=%#x,%c\r\n", i, buffer[i], buffer[i]);
	}
}

__task void process_uart3(void)
{
	u32 index;
	char transpond[][30] =
	{
		"\r\nOK\r\n",
		"\r\nSEND OK",
		"AT\r\n",
		"AT+CSQ",
		"AT+CIPHEAD",
		"AT+CIPSTART",
		"AT+MORING",
		"AT+CIPSEND",
		"AT+CIPSTATUS",
		"AT+CIPCLOSE",
		"AT+CIPSHUT",
		"AT+CIPMUX",
		"AT+CIPRXGET",
		"AT+CIPQRCLOSE",
		"AT+CIPMODE",
		"AT+CBC",
	};
	
	while(1)
	{
		int ret = 0, i = 0;
		char command[256] = "";
		char *msg = NULL;
		char *receive = NULL;
		
		ret = read_uart_data(&uart3_data, command, sizeof(command));
		if(ret > 0)
		{
			index++;
			if(charger.uart3_debug == ON)
			{
				printf("\r\n**************************************\r\n");
				printf("index = %d, receive sim900a[%d]=[%s]\r\n", index, ret, command);
				print_buffer(command, ret);
				printf("\r\n**************************************\r\n");
			}
			else
			{
				printf("\r\n**************************************\r\n");
				printf("index = %d, receive sim900a[%d]=[%s]\r\n", index, ret, command);
				printf("\r\n**************************************\r\n");
			}
			//分发和处理接收到的信息
			for(i=0;i<ARRAY_SIZE(transpond);i++)
			{			
				if(strncmp(command, transpond[i], strlen(transpond[i])) == 0)
				{
					//printf("[[%s]] matched\r\n", transpond[i]);
					msg = (char *)calloc(1, ret);
					memcpy(msg, command, ret);
					os_mbx_send(&sim900a_receive, msg, 0);
				}
				else
				{
					//printf("[[%s]] not matched\r\n", transpond[i]);
				}
			}
			receive = strstr(command, "+IPD,");
			if(receive != NULL)
			{
				printf("receive message[%s]\r\n", receive+strlen("+IPD,"));
			}
		}
	}
}

__task void rtx_init (void) 
{
	os_tsk_prio_self (20);//Set the priority

	board_init();

	tid_task_1 =  os_tsk_create_user(process_uart1,14,task_stack_1,sizeof(task_stack_1));
	tid_task_2 =  os_tsk_create_user(process_uart3,14,task_stack_2,sizeof(task_stack_2));

	os_tsk_delete_self (); 
}

int main(void)
{
	os_sys_init(rtx_init);//操作系统初始化
	return 0;
}

