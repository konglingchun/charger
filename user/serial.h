#ifndef _SERIAL_H_
#define _SERIAL_H_

#define UART_BUFFER_LEN 256

typedef enum
{
	UART_CACHE_EMPTY = 0,
	UART_CACHE_FULL,
	UART_CACHE_LOAD,
}uart_status_t;

typedef struct
{
	u8 receive[UART_BUFFER_LEN];
	u8 length;
	uart_status_t status;
}uart_t;

#define DEBUG_USART USART1

extern uart_t uart1_data;
extern uart_t uart2_data;
extern uart_t uart3_data;

extern void uart1_init(void);

extern void uart2_init(void);

extern void uart3_init(void);

extern void uart_data_init(uart_t *data);

extern void uart1_nvic_init(void);

extern void uart2_nvic_init(void);

extern void uart3_nvic_init(void);

extern void uart1_send_str(char *str, u16 length);

extern void uart2_send_str(char *str, u16 length);

extern void uart3_send_str(char *str, u16 length);

extern int read_uart_data(uart_t *data, char *buffer, int buffer_size);

#endif

