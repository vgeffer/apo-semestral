///@file gpio.h
#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

///Logical low pin state 
#define PIN_LOW 0
///Logical high pin state
#define PIN_HI 1

///Number of GPIO present on the board
#define BOARD_PIN_COUNT 22

int gpio_init(void);
int set_pin(uint8_t pin, uint8_t state);

#endif