#include <stdio.h>
#include <string.h>

#include "command.h"

//��������������
int execute_command(char *command, command_t *list, int list_size)

{
	char *argv[128] = {NULL};
	unsigned char argc = 0;
	unsigned char i = 0;
	char command_buffer[256] = "";
	
	if(strlen(command) == 0)
	{
		return -1;
	}
	else if(strlen(command) > sizeof(command_buffer))
	{
		return -2;
	}
	else
	{
		memcpy(command_buffer, command, strlen(command));
	}
		
	argv[0] = command_buffer;
	while((argv[argc]=strtok(argv[argc], " "))!=NULL)//���ڱ��ָ���ַ����з��ַָ��ַ�ʱ�������ַ���Ϊ'\0'��
	{
        //printf("argv[%d] = %s\n", argc, argv[argc]);
        argc++;
		if(argc > 128)//Too many argument
		{
			return -3;
		}
	}

	if(argc == 0)//No orders
	{
		return -4;
	}

	for(i=0;i<list_size;i++)
	{
		//��������
		//printf("list[%d].cmd_name=[%s], argv=[%s]\r\n", i, list[i].cmd_name, argv[0]);
		if(strcmp(list[i].cmd_name, argv[0])==0)
		{
			//ִ������
			list[i].cmd_func(argc, argv);
			return 0;
		}
	}	
	return -5;
}

#if 0
void command_func_demo(int argc, char *argv[])
{
	int i;

	for(i=0; i<argc; i++)
	{
		printf("argv[%d]=[%s]\r\n", i, argv[i]);
	}
}

//�������飺�������� �������ʹ�������
command_t command_list_demo[]=
{	
	{"demo", command_func_demo},
};

int command_execute_demo(char *command)
{
	int ret;
	
	ret = execute_command(command,
						command_list_demo,
						ARRAY_SIZE(command_list_demo));
	return ret;
}
#endif

