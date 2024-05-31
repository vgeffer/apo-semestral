#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

#define PIN_LOW 0
#define PIN_HI 1

#define BOARD_PIN_COUNT 22

int gpio_init(void);
int set_pin(uint8_t pin, uint8_t state);


#endif