#ifndef __CONFIG_H__
#define __CONFIG_H__


#ifndef UART_PORT
    #define UART_PORT "/dev/ttyS0"
#endif

#ifndef SCREEN_CONFIG
    #define SCREEN_SDA 3
    #define SCREEN_CLK 4
    #define SCREEN_CLR 5
    #define SCREEN_WR 6
    #define SCREEN_RS 7
    #define SCREEN_CS 8
#endif

#endif