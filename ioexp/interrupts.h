///@file interrupts.h
#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <avr/interrupt.h>

///D
#define disable_intr    cli
#define enable_intr     sei

uint8_t get_intr_flag(void);
void set_intr_func(void (*intr_fnc)(void));

#endif