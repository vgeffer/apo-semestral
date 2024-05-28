#ifndef __GPIO_H__
#define __GPIO_H__

#include "../avrdefs.h"
#include <avr/io.h>

#define LED_BUILTIN 13 

#define PIN_IN  0
#define PIN_OUT 1

#define PIN_LOW 0
#define PIN_HI  1

int set_pin_type(uint8_t pin, uint8_t state);
int set_pin_state(uint8_t pin, uint8_t state);
int get_pin_state(uint8_t pin);

#endif