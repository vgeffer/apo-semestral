#ifndef __UART_H__
#define __UART_H__

typedef struct {

    int uart_fd;
} uart_t;

int uart_init(uart_t* port, const char* dev, int baud, int parity);
int uart_putc(uart_t* port, char c);
char uart_getc(uart_t* port);


#endif