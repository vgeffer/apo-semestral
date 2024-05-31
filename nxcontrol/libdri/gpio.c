#include "gpio.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <nuttx/ioexpander/gpio.h>

static int gpio_fds[BOARD_PIN_COUNT];
static char dev_name[] = "/dev/gpio\00\00"; /* With extra NULL bytes to be replaced */

/* Will only work for 2-digit number of io pins */
static void to_dev_name(int pin) {

    /* Reset dev_name */
    dev_name[9] = '\00';
    dev_name[10] = '\00';

    if ( pin < 0 || pin > 99) return;

    if ( pin < 10 ) {

        dev_name[9] = '0' + pin;
        return;
    } 

    dev_name[10] = '0' + (pin % 10);
    dev_name[9] = '0' + ((pin - (pin % 10)) / 10); 
}

int gpio_init(void) {

    memset(gpio_fds, 0xFF, sizeof(gpio_fds));

    for ( int i = 0; i < BOARD_PIN_COUNT; i++ ) {

        to_dev_name(i);
        int fd_gpio = open(dev_name, O_RDWR);
        
        if ( fd_gpio < 0 ) return -1;
        gpio_fds[i] = fd_gpio;
    }

    return 0;
}

int set_pin(uint8_t pin, uint8_t state) {

    if ( pin >= BOARD_PIN_COUNT ) return -1;
    if ( gpio_fds[pin] < 0 ) return -1; /* Sanity check */

    return ioctl(gpio_fds[pin], GPIOC_WRITE, (unsigned long)state); 
}