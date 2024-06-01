///@file gpio.h
#ifndef __GPIO_H__
#define __GPIO_H__

#include "../avrdefs.h"
#include <avr/io.h>

///Pin of the Arduino Uno's builtin LED
#define LED_BUILTIN 13 

///Input pin
#define PIN_IN  0
///Output pin
#define PIN_OUT 1

///Logical low pin state
#define PIN_LOW 0
///Logical high pin state
#define PIN_HI  1

int set_pin_type(uint8_t pin, uint8_t state);
int set_pin_state(uint8_t pin, uint8_t state);
int get_pin_state(uint8_t pin);

#endif