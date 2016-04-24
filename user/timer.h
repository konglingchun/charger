#ifndef __TIMER_H_
#define __TIMER_H_

typedef enum
{
	TIMER_OFF = 0,//stop timer
	TIMER_ON,//start timer
	TIME_OUT_TRUE,//time out
	TIME_OUT_FALSE,//time not out 
}time_trigger_t;

typedef struct
{
	u16 day;//0-356
	u16 hour;//0-23
	u16 minute;//0-59
	u16 second;//0-59
	u16 msec;//0-999
	u16 day_reload;
	u16 hour_reload;
	u16 minute_reload;
	u16 second_reload;
	u16 msec_reload;
	time_trigger_t start;
	time_trigger_t time_out;
}system_timer_t;

extern system_timer_t uart1_time_trigger;
extern system_timer_t uart2_time_trigger;
extern system_timer_t uart3_time_trigger;

extern void timer2_init(u16 arr, u16 psc);
	
extern int system_timer_init(
		system_timer_t *time,
		u16 day,
		u16 hour,
		u16 minute,
		u16 second,
		u16 msec);

extern int system_timer_reload(system_timer_t *time)	;

extern int system_timer_start(system_timer_t *time);

extern int system_timer_stop(system_timer_t *time);

extern int is_system_timer_timer_on(system_timer_t *time);

extern int is_system_timer_timeout(system_timer_t *time);

extern int system_timer_clear_timeout(system_timer_t *time);	

extern int system_timer_tick(system_timer_t *time);

extern int system_timer_test(system_timer_t *time);

#endif
