#ifndef __DRW_H__
#define __DRW_H__

#include "screen.h"
#include "font.h"

#define FROM_RGB(r, g, b)  ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

typedef struct {
    char c;
    uint16_t col;
} scr_char_t;

enum colors {

    BLACK   = FROM_RGB(0, 0, 0),
    RED     = FROM_RGB(255, 0, 0),
    GREEN   = FROM_RGB(0, 255, 0),
    BLUE    = FROM_RGB(0, 0, 255),
    ORANGE  = FROM_RGB(0xFC, 0xAC, 0x34),
    WHITE   = FROM_RGB(255, 255, 255)
};

void draw(screen_t* scr);

#endif