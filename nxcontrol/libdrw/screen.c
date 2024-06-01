///@file screen.c
#include "screen.h"
#include "../libdri/gpio.h"
#include <unistd.h>

/* This boot table has been borrowed from */
/* https://github.com/adafruit/Adafruit_ILI9341/blob/master/Adafruit_ILI9341.cpp */
static const uint8_t display_bootcode[] = {
    0xEF,       3,      0x03, 0x80, 0x02,
    0xCF,       3,      0x00, 0xC1, 0x30,
    0xED,       4,      0x64, 0x03, 0x12, 0x81,
    0xE8,       3,      0x85, 0x00, 0x78,
    0xCB,       5,      0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7,       1,      0x20,
    0xEA,       2,      0x00, 0x00,
    PWCTL1,     1,      0x23,
    PWCTL2,     1,      0x10,
    VCOMCTL1,   2,      0x3e, 0x28,
    VCOMCTL2,   1,      0x86,
    MADCTL,     1,      0x48,
    VSADDR,     1,      0x00,
    PIXFMT,     1,      0x55,
    FRMCTLN,    2,      0x00, 0x18,
    DFNCTL,     3,      0x08, 0x82, 0x27,
    0xF2,       1,      0x00,  
    GAMMA,      1,      0x01,
    GMCORP,     15,     0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    GMCORN,     15,     0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    SLPOUT,     0x80,
    DON,        0x80,
    0x00                                   
};

/* Write data to screen */
static void write_scr(screen_t* scr, uint16_t data) {

    sh_writew(scr->scr_out, data);

    set_pin(scr->cs, PIN_LOW);
    usleep(1);
    set_pin(scr->cs, PIN_HI);
}

/**
 * Enable r/w data to/from the screen
 * @param[in] scr Screen
*/
void scr_enable(screen_t* scr) {

    set_pin(scr->cs, PIN_LOW);
    usleep(1);
}

/**
 * Disable r/w data to/from the screen
 * @param[in] scr Screen
*/
void scr_disable(screen_t* scr) {

    usleep(1);
    set_pin(scr->cs, PIN_HI);
}

/**
 * Write command to the screen
 * @param[in] scr Screen
 * @param[in] cmd Command to send
*/
void scr_command(screen_t* scr, uint16_t cmd) {

    set_pin(scr->data, PIN_LOW);
    write_scr(scr, cmd);
}

/**
 * Write data to the screen
 * @param[in] src Screen
 * @param[in] data Data to write
*/
void scr_data(screen_t* scr, uint16_t data) {

    set_pin(scr->data, PIN_HI);
    write_scr(scr, data);
}

/**
 * Initialises the screen and runs the boot code
 * @param[in] scr Screen
 * @param[in] wr Write Enable pin
 * @param[in] data Register Select pin
 * @param[in] cs Chip Enable pin
*/
void scr_init(screen_t* scr, uint8_t wr, uint8_t data, uint8_t cs) {

    if ( !scr )
        return;

    if ( !scr->scr_out )
        return;

    scr->cs = cs;
    scr->wr = wr;
    scr->data = data;

    uint8_t cmd;
    uint8_t idx = 0;
    
    scr_enable(scr);

    /* Reset screen */
    scr_command(scr, SWRST);
    usleep(500000); /* Wait 500ms after reset */

    while ( (cmd = display_bootcode[idx++]) > 0 ) {

        /* Write command */
        scr_command(scr, cmd);

        uint8_t argc = display_bootcode[idx++]; 

        /* If bit 7 is set, wait 150ms (required by some commands) */
        if ( argc & 0x80 )
            usleep(150000); /* 150 ms*/

        argc &= 0x7F;

        /* Write arg list */
        for ( ; argc > 0; argc-- )
            scr_data(scr, display_bootcode[idx++]);
    }
    scr_disable(scr);
}