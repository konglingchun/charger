#ifndef __MIAN_H_
#define __MIAN_H_

#include "stm32f10x.h"

typedef enum
{
	ON = 0,
	OFF,
}switcher_t;

typedef enum
{
	TCP = 0,
	UDP,
}protocol_t;

typedef enum
{
	NONE = 0,
	AES,
	WEP,
	SSL,
}encrypt_t;

typedef union
{
	u8 id[12];
	struct
	{
		u32 id[3];
	}arry;
}chip_id_t;

typedef struct
{
	switcher_t sim900a_power;//sim900a开关机状态
	switcher_t uart3_debug;
	protocol_t protocol;//通信协议类型
	char tel_num[12];//手机号
	u16 message_index;//消息流水号
	u16 reply_index;//应答流水号
	encrypt_t encrypt;//加密规则
	chip_id_t chip_id;//芯片ID
	u8 server_ip[4];//4个字节，从高位到低位，依次对应IP地址的4个段
	u16 server_port;//端口地址，先高位后低位
	u32 frequence;//单位秒
}charger_t;

extern charger_t charger;;

#endif

