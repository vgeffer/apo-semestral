///@file uart.c
#include "uart.h"
#include "../common.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * Initialises serial terminal over UART
 * @param[in] port UART port
 * @param[in] uart_dev UART device
 * @param[in] baud Termios baud rate constant
 * @param[in] parity Termios parity constatnt
 * @return E_OK on success, otherwise error
*/
int uart_init(uart_t* port, const char* uart_dev, int baud, int parity) {

    if ( !port ) return E_INVAL;

    port->uart_fd = open(uart_dev, O_RDWR | O_NOCTTY | O_SYNC);
    if ( port->uart_fd < 0 ) return E_IOERR;

    struct termios tty_uart;
    if ( tcgetattr(port->uart_fd, &tty_uart) != 0 ) return E_IOERR;

    cfsetospeed(&tty_uart, baud);
    cfsetispeed(&tty_uart, baud);

    /* Set 8-bit data */
    tty_uart.c_cflag = (tty_uart.c_cflag & ~CSIZE) | CS8;            
    tty_uart.c_cflag |= (CLOCAL | CREAD);
    tty_uart.c_cflag &= ~(PARENB | PARODD);
    tty_uart.c_cflag |= parity;
    tty_uart.c_cflag &= ~CSTOPB;
    tty_uart.c_cflag &= ~CRTSCTS;

    
    tty_uart.c_iflag &= ~IGNBRK;               
    tty_uart.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty_uart.c_cc[VMIN]  = 0; 
    tty_uart.c_cc[VTIME] = 5;
    
    tty_uart.c_lflag = 0;
    tty_uart.c_oflag = 0;     
    

    if ( tcsetattr(port->uart_fd, TCSANOW, &tty_uart) != 0 ) return E_IOERR;    
    return E_OK;
}

/**
 * Put character to UART console
 * @param[in] port UART port
 * @param[in] c Character
 * @return 0 on success, 1 otherwise
*/
int uart_putc(uart_t* port, char c) { 

    int ret = write(port->uart_fd, &c, 1); 
    usleep(100); /* Wait approx 100us per char */
    return ret;
}

/**
 * Get chacter from UART console
 * @param[in] port UART port
 * @return -1 on error, otherwise read character
*/
char uart_getc(uart_t* port) { 

    char c;    
    int ret = read(port->uart_fd, &c, 1); 
    usleep(100); /* Wait approx 100us per char */

    if ( ret < 0 ) return E_IOERR;
    return c;
}
