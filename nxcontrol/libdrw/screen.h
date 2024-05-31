#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdint.h>
#include "../libdri/shiftreg.h"
#include "../utils/circbuffer.h"

#define SCREEN_WIDTH 320 /* In PX */
#define SCREEN_HEIGHT 240

enum screen_commands {

    NOP         = 0x00,
    SWRST       = 0x01,
    RDID        = 0x04,
    RDST        = 0x09,

    RDPMD       = 0x0A,
    RDMADCTL    = 0x0B,
    RDPXFT      = 0x0C,
    RDIMFT      = 0x0D,
    RDDIAG      = 0x0F,

    SLPON       = 0x10,
    SLPOUT      = 0x11,
    PRTON       = 0x12,
    NORON       = 0x13,
    INVOFF      = 0x20,
    INVON       = 0x21,
    GAMMA       = 0x26,
    DOFF        = 0x28,
    DON         = 0x29,

    COADDR      = 0x2A,
    PGADDR      = 0x2B,
    DMWR        = 0x2C,
    DMRD        = 0x2E,

    PTLAR       = 0x30,
    VSDEF       = 0x33,
    MADCTL      = 0x36,
    VSADDR      = 0x37,
    IDLEOFF     = 0x38,
    IDLEON      = 0x39,
    PIXFMT      = 0x3A,
    DMWRC       = 0x3C,
    DMRDC       = 0x3E,

    FRMCTLN     = 0xB1,
    FRMCTLI     = 0xB2,
    FRMCTLP     = 0xB3,
    INVCTL      = 0xB4,
    DFNCTL      = 0xB6,

    PWCTL1      = 0xC0,
    PWCTL2      = 0xC1,
    VCOMCTL1    = 0xC5,
    VCOMCTL2    = 0xC7,

    RDID1       = 0xDA,
    RDID2       = 0xDB,
    RDID3       = 0xDC,
    RDID4       = 0xDD,

    GMCORP      = 0xE0,
    GMCORN      = 0xE1,
};

typedef struct {

    circ_buffer_t* lines;
    uint16_t bg_color;

    shiftreg_t* scr_out;
    uint8_t cs;
    uint8_t wr;
    uint8_t data;

} screen_t;

void scr_init(screen_t* scr, uint8_t wr, uint8_t data, uint8_t cs);

void scr_enable(screen_t* scr);
void scr_disable(screen_t* scr);
void scr_command(screen_t* scr, uint16_t cmd);
void scr_data(screen_t* scr, uint16_t data);

#endif