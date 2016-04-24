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
	switcher_t sim900a_power;//sim900a���ػ�״̬
	switcher_t uart3_debug;
	protocol_t protocol;//ͨ��Э������
	char tel_num[12];//�ֻ���
	u16 message_index;//��Ϣ��ˮ��
	u16 reply_index;//Ӧ����ˮ��
	encrypt_t encrypt;//���ܹ���
	chip_id_t chip_id;//оƬID
	u8 server_ip[4];//4���ֽڣ��Ӹ�λ����λ�����ζ�ӦIP��ַ��4����
	u16 server_port;//�˿ڵ�ַ���ȸ�λ���λ
	u32 frequence;//��λ��
}charger_t;

extern charger_t charger;;

#endif

