#ifndef __PROTUTILS_H__
#define __PROTUTILS_H__

#include "../libdri/uart.h"

#include <stdint.h>

int await_recv(uart_t* port, int max_timeout);
int send_packet(uart_t* port, uint16_t cmd, uint64_t data);
int send_retry_last(uart_t* port);

int recv_waitfor_packet(uart_t* port, uint16_t* cmd, uint64_t* data, int max_timeout);
int recv_packet(uart_t* port, uint16_t* cmd, uint64_t* data);


#endif


