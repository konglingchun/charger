#ifndef _SIM900A_H_
#define _SIM900A_H_

#include <RTL.h>

#define SIM900A_COMMAND_OK 0
#define SIM900A_COMMAND_ERROR -1


extern os_mbx_declare(sim900a_receive, 20);

extern char get_ctrl_code(char code);

extern void sim900_power_pin_init(void);

extern void sim900_net_pin_init(void);

extern void sim900a_power_on(void);

extern void sim900a_power_off(void);

extern void sim900a_init(void);

extern void sim900a_command_result(char *command, int ret);

extern int sim900a_send(char *command, int cmd_size, int retry_times, U16 timeout);

extern int sim900a_querry(char *command, int cmd_size, char *querry, int querry_size, int retry_times, U16 timeout);

#endif

