
/* Uart driver */
#ifndef __UART_H__
#define __UART_H__

#include "../avrdefs.h"

int uart_init();
int uart_transmit(uint16_t cmd, uint64_t buf, uint8_t flags);
int uart_recv(uint16_t* cmd, uint64_t* buf, uint8_t flags);

#endif