///@file avrdefs.h
#ifndef __AVRDEFS_H__
#define __AVRDEFS_H__

#include <stdint.h>
#include <stddef.h>

/* UART setup */
///Baud rate of the uart port
#define BAUD UPROT_BAUD
///Frequency of the MCU's CPU
#define F_CPU 16000000UL
#include <util/delay.h>

/**
 * List of errors
*/
enum ERRORS {
    E_OK        = 0,
    E_INVALID   = -1,
    E_NULLPTR   = -2,
    E_INTR      = -3,
    E_AGAIN     = -4,
    E_TIMEOUT   = -5
};

/**
 * List of function flags
*/
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

#endif