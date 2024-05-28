#ifndef __AVRDEFS_H__
#define __AVRDEFS_H__

#include <stdint.h>

/* UART setup */
#define BAUD UPROT_BAUD
#define F_CPU 16000000UL
#include <util/delay.h>


enum ERRORS {
    E_OK        = 0,
    E_INVALID   = -1,
    E_NULLPTR   = -2,
    E_INTR      = -3,
    E_AGAIN     = -4,
    E_TIMEOUT   = -5
};

enum OPFLAGS {
    F_NONBLOCK  = 1 << 0,
    F_NOINTR    = 1 << 1,
    F_RETRY     = 1 << 2,
    F_FLG3 = 1 << 3,
    F_FLG4 = 1 << 4,
    F_FLG5 = 1 << 5,
    F_FLG6 = 1 << 6,
    F_FLG7 = 1 << 7
};

#define NULL (void*)0

static const unsigned int __nan = 0xFFFFFFFF;
#define NaN *((float*)&__nan)

#define FATAL_ERR() { char f = 1; while( 1 ) set_pin_state(LED_BUILTIN, (f ^= 1)); _delay_ms(500); }

#endif