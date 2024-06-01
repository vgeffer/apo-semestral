///@file drw.h
#ifndef __DRW_H__
#define __DRW_H__

#include "screen.h"
#include "font.h"

///Utility to convert from 24 bit RGB to 16 bit 565 RGB
#define FROM_RGB(r, g, b)  ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

/**
 * Structure to hold character and color buffers to be drawn to the screen
*/
typedef struct {
    ///Character
    char c;
    ///Foreground color
    uint16_t col;
} scr_char_t;

void draw(screen_t* scr);

#endif