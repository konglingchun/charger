#include <stdio.h>
#include <string.h>
#include <RTL.h>

#include "command.h"
#include "charger_command.h"
#include "charger.h"
#include "sim900a.h"
#include "serial.h"

void command_uart3_debug_on(int argc, char *argv[])
{
	charger.uart3_debug = ON;
	printf("will print messages received form uart3\r\n");
}

void command_uart3_debug_off(int argc, char *argv[])
{
	charger.uart3_debug = OFF;
	printf("will not print messages received form uart3\r\n");
}

void command_power_on(int argc, char *argv[])
{
	sim900a_power_on();
}

void command_power_off(int argc, char *argv[])
{
	sim900a_power_off();
}

void command_protocol_tcp(int argc, char *argv[])
{
	printf("protocol now is TCP\r\n");
	charger.protocol = TCP;
}

void command_protocol_udp(int argc, char *argv[])
{
	printf("protocol now is UDP\r\n");
	charger.protocol = UDP;
}

void command_setip(int argc, char *argv[])
{
	int server_ip[4];//4个字节，从高位到低位，依次对应IP地址的4个段
	
	if(argc == 2)
	{
		sscanf(argv[1], "%d.%d.%d.%d", &server_ip[3], &server_ip[2], &server_ip[1], &server_ip[0]);
		if((server_ip[3] >= 0)&&(server_ip[3] <= 255)
			&&(server_ip[2] >= 0)&&(server_ip[2] <= 255)
			&&(server_ip[1] >= 0)&&(server_ip[1] <= 255)
			&&(server_ip[0] >= 0)&&(server_ip[0] <= 255))
		{
			
		}
		else
		{
			printf("ip address wrong\r\n");
			return;
		}
		printf("ip = %d.%d.%d.%d\r\n", server_ip[3], server_ip[2], server_ip[1], server_ip[0]);
		charger.server_ip[0] = server_ip[0];
		charger.server_ip[1] = server_ip[1];
		charger.server_ip[2] = server_ip[2];
		charger.server_ip[3] = server_ip[3];
	}
	else
	{
		printf("wrong setip format\r\n");
	}
}

void command_setport(int argc, char *argv[])
{
	int server_port;//端口地址，先高位后低位
	
	if(argc == 2)
	{
		sscanf(argv[1], "%d", &server_port);
		printf("port = %d\r\n", server_port);
		memcpy(&charger.server_port, &server_port, sizeof(server_port));
	}
	else
	{
		printf("wrong setport format\r\n");
	}
}

void command_signal(int argc, char *argv[])
{
	int ret;
	char querry[30] = "";
	int signal = 0;
	
	ret = sim900a_querry("AT+CSQ", strlen("AT+CSQ"), querry, sizeof(querry), 3, 20);
	if(ret >= 0)
	{
		//printf("signal = [%s]\n", querry);
		sscanf(querry, "+CSQ: %d", &signal);
		//printf("signal = %d\r\n", signal);
		if(signal == 0)
		{
			printf("signal <= -115dBm\r\n");
		}
		else if(signal == 1)
		{
			printf("signal == -111dBm\r\n");
		}
		else if(signal >= 0
			&&signal <= 30)
		{
			printf("signal (-111 ~ -54)dBm\r\n");
		}
		else if(signal == 31)
		{
			printf("signal >= -52dBm\r\n");
		}
		else if(signal == 99)
		{
			printf("signal unknown\r\n");
		}
	}
}

void command_at(int argc, char *argv[])
{
	int ret;

	ret = sim900a_send("AT", strlen("AT"), 3, 20);
	sim900a_command_result("AT", ret);
}

void command_at_init(int argc, char *argv[])
{
	int ret;
	int i;
	char command[][30] =
	{
		"AT",
		"AT+CIPMUX=0",
		"AT+CIPRXGET=1",
		"AT+CIPQRCLOSE=1",
		"AT+CIPMODE=0",
		"AT+MORING=1",
	};
	
	for(i=0;i<ARRAY_SIZE(command);i++)
	{
		ret = sim900a_send(command[i], strlen(command[i]), 3, 20);
		sim900a_command_result(command[i], ret);
	}
}

void command_call(int argc, char *argv[])
{
	int i = 0;
	int phone_num_length = 0;
	char buffer[20] = "";
	int ret;	
	
	if(argc == 2)
	{
		phone_num_length = strlen(argv[1]);
		if(phone_num_length > 12)
		{
			printf("phone number too long\r\n");
			return;
		}
		printf("argv[1] = %s\r\n", argv[1]);
		for(i=0;i<phone_num_length;i++)
		{
			if((argv[1][i] >= '0')
				&&(argv[1][i] <= '9'))
			{
				;				
			}
			else
			{
				printf("wrong phone number\r\n");
				return;
			}
		}
		snprintf(buffer, sizeof(buffer), "ATD%s;", argv[1]);
		ret = sim900a_send(buffer, strlen(buffer), 30, 20);
		sim900a_command_result(buffer, ret);
	}
	else
	{
		printf("wrong call format\r\n");
	}
}

void command_hangup(int argc, char *argv[])
{
	int ret;
	
	ret = sim900a_send("ATH", strlen("ATH"), 3, 20);
	sim900a_command_result("ATH", ret);
}

void command_pickup(int argc, char *argv[])
{
	int ret;
	
	ret = sim900a_send("ATA", strlen("ATA"), 3, 20);
	sim900a_command_result("ATA", ret);
}

void command_status(int argc, char *argv[])
{
	int ret;
	
	ret = sim900a_send("AT+CIPSTATUS", strlen("AT+CIPSTATUS"), 3, 20);
	sim900a_command_result("AT+CIPSTATUS", ret);
}

void command_connect(int argc, char *argv[])
{
	char buffer[50] = "";
	int ret;
	
	ret = sim900a_send("AT+CIPHEAD=1", strlen("AT+CIPHEAD=1"), 3, 20);
	sim900a_command_result("AT+CIPHEAD=1", ret);
	os_dly_wait(80);
	//AT+CIPSTART="TCP","58.215.235.62","61233"
	if(charger.protocol == TCP)
	{
		snprintf(buffer, sizeof(buffer), 
			"AT+CIPSTART=\"%s\",\"%d.%d.%d.%d\",\"%d\"",
			"TCP",
			charger.server_ip[3],
			charger.server_ip[2],
			charger.server_ip[1],
			charger.server_ip[0],
			charger.server_port);
		ret = sim900a_send(buffer, strlen(buffer), 3, 20);
		sim900a_command_result(buffer, ret);
	}
	else if(charger.protocol == UDP)
	{
		snprintf(buffer, sizeof(buffer), 
			"AT+CIPSTART=\"%s\",\"%d.%d.%d.%d\",\"%d\"",
			"UDP",
			charger.server_ip[3],
			charger.server_ip[2],
			charger.server_ip[1],
			charger.server_ip[0],
			charger.server_port);
		ret = sim900a_send(buffer, strlen(buffer), 3, 20);
		sim900a_command_result(buffer, ret);
	}
}

void command_send(int argc, char *argv[])
{
	char end = 0;
	int ret;
	
	if(argc == 2)
	{
		ret = sim900a_send("AT+CIPSEND", strlen("AT+CIPSEND"), 3, 20);
		sim900a_command_result("AT+CIPSEND", ret);
		os_dly_wait(80);
		uart3_send_str(argv[1], strlen(argv[1]));
		end = get_ctrl_code('Z');
		uart3_send_str(&end, 1);
		//ret = sim900a_send(&end, 1, 30, 20);
		//sim900a_command_result("CTRL+Z", ret);
	}
	else
	{
		printf("wrong sen format\r\n");
	}
}

void command_close_connect(int argc, char *argv[])
{
	int ret;
	
	ret = sim900a_send("AT+CIPCLOSE=1", strlen("AT+CIPCLOSE=1"), 3, 20);
	sim900a_command_result("AT+CIPCLOSE=1", ret);
}

void command_close_ip(int argc, char *argv[])
{
	int ret;
	
	ret = sim900a_send("AT+CIPSHUT", strlen("AT+CIPSHUT"), 3, 20);
	sim900a_command_result("AT+CIPSHUT", ret);
}

void command_battery(int argc, char *argv[])
{
	int ret;
	
	ret = sim900a_send("AT+CBC", strlen("AT+CBC"), 3, 20);
	sim900a_command_result("AT+CBC", ret);
}

command_t charger_command_list[]=
{	
	{"debugon", command_uart3_debug_on},
	{"debugoff", command_uart3_debug_off},
	{"poweron", command_power_on},
	{"on", command_power_on},
	{"poweroff", command_power_off},
	{"off", command_power_off},
	{"settcp", command_protocol_tcp},
	{"setudp", command_protocol_udp},
	{"setip", command_setip},
	{"setport", command_setport},
	{"signal", command_signal},
	{"at", command_at},
	{"atinit", command_at_init},
	{"call", command_call},
	{"hangup", command_hangup},
	{"pickup", command_pickup},
	{"status", command_status},
	{"connect", command_connect},
	{"send", command_send},
	{"close", command_close_connect},
	{"close_ip", command_close_ip},
	{"battery", command_battery},
};

int charger_command(char *command)
{
	int ret;
	
	ret = execute_command(command,
						charger_command_list,
						ARRAY_SIZE(charger_command_list));
	return ret;
}

