#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "stm32f10x.h"

//传输规则:先传高位再传低位

typedef enum
{
	LOAD_INFO = 0x0001,//charger->server
	CHARGER_STATUS = 0x0002,//charger->server
	STATUS_REQUEST = 0x0802,//server->charger
	STATUS_RESPONSE = 0x8802,//charger->server
	UPDATE_REQUEST = 0x0803,//server->charger
	UPDATE_RESPONSE = 0x8803,//charger->server
	CHANGE_SERVER_REQUEST = 0x0804,//server->charger
	CHANGE_SERVER_RESPONSE = 0x8804,//charger->server
	SEND_FREQUENCE_REQUEST = 0x0805,//server->charger
	SEND_FREQUENCE_RESPONSE = 0x8805,//charger->server
}message_id_t;

typedef struct
{
	u8 identifier;
	
}message_t;


#endif

