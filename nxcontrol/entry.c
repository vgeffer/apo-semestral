///@file entry.c
#include "common.h"
#include "config.h"
#include "libdri/gpio.h"
#include "libdri/shiftreg.h"
#include "libdri/uart.h"
#include "libdrw/screen.h"
#include "libdrw/scrprint.h"
#include "uprot/protdefs.h"
#include "uprot/protutils.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define STATE_OK 0
#define STATE_EMERGENCY 1
#define STATE_MISMATCH 2
#define STATE_HWERR 3

uart_t UART0 = { .uart_fd = -1 };
shiftreg_t scr_shr = {
    .p_clk = SCREEN_CLK,
    .p_sda = SCREEN_SDA,
    .p_clr = SCREEN_CLR
};
screen_t scr;

float current_temp = NAN; 
float current_humid = NAN;

int current_state = 0;

/**
 * Updates the screen based on the collected data
*/
void update_screen(void) {

    char buffer [8]; /* XXXXX.X\00, for float-string converisons */
    clear_screen(&scr);

    print(&scr, "Measured temperature: ");
    sprintf(buffer, "%.1f", current_temp);
    print(&scr, buffer);
    print(&scr, "\n");

    print(&scr, "Measured humidity: ");
    sprintf(buffer, "%.1f", current_humid);
    print(&scr, buffer);
    print(&scr, "\n");

    switch (current_state) {
        
        case STATE_EMERGENCY:
            print(&scr, "\x1b\02Dangerously hisgh temperatures measured!\x1b\01\n");
            break;

        case STATE_HWERR:
            print(&scr, "\x1b\04Warning! Unable to take one or more measurements! Please check instruments for possible HW failure!\x1b\01\n");
            break;

        case STATE_MISMATCH:
            print(&scr, "\x1b\04Warning! Mismatch between temperature sensors detected! Possible HW failure!\x1b\01\n");
            break;

        case STATE_OK:
            print(&scr, "\x1b\03All system runing\x1b\01\n");
            break;
        default:
            break;
    }

}

/**
 * Requests and processes updated data from IOExpander
 * @param[out] 0 on succes, otherwise error code
*/
int ioexp_update(void) {

    uint16_t cmd = 0;
    uint64_t data = 0;

    union {
        struct {
            uint32_t pad; /* Pad out upper 32 bits */
            float data; /* Actual data */
        } __attribute__((packed)) floats;
        uint64_t buff;
    } buff_to_float;

    do {
        if ( send_packet(&UART0, CMD_TEMP, 0LU) < E_OK ) return E_IOERR;
        if ( recv_waitfor_packet(&UART0, &cmd, &data, 100) != E_OK ) return E_IOERR;
    } while ( cmd == CMD_ERR );

    buff_to_float.buff = data;
    current_temp = buff_to_float.floats.data; 

    do {
        if ( send_packet(&UART0, CMD_HUMID, 0LU) < 0 ) return E_IOERR;
        if ( recv_waitfor_packet(&UART0, &cmd, &data, 100) != 0 ) return E_IOERR;
    } while ( cmd == CMD_ERR );

    buff_to_float.buff = data;
    current_humid = buff_to_float.floats.data; 

    return 0;
}

/**
 * Entry point of the app
*/
int main(void) {

    /* Create screen */
    scr.bg_color = 0x00;
    scr.scr_out = &scr_shr;

    /* Init systems */
    if ( gpio_init() < 0 ) return 1;
    if ( uart_init(&UART0, UART_PORT, UPROT_BAUD, 0) < 0 ) return 1;
    scr_init(&scr, SCREEN_WR, SCREEN_RS, SCREEN_CS); 

    /* MAIN LOOP */
    while ( 1 ) {

        /* Wait for 500 ms for any incoming message from ioexp */
        int ret = await_recv(&UART0, 500);
        if ( ret < 0 ) break;

        if ( ret == 1 ) {

            /* No packet received, do an inquiry */
            if ( ioexp_update() < 0) break;
            update_screen();
        }

        /* Some packet was received, parse it */
        uint16_t cmd = CMD_INVAL;
        uint64_t data = 0;

        /* If received incorrectly, request a retry */
        if ( recv_packet(&UART0, &cmd, &data) < 0 )
            send_packet(&UART0, CMD_ERR, 0UL);

        /* These are the only packtets that the ioexp is able to send by itself without being prompted to */
        switch ( cmd ) {
            
            case CMD_EMERGENCY: {

                /* Emergency occured, change state */
                current_state = STATE_EMERGENCY;
                break;
            }

            case CMD_HW_ERR: {

                /* Read failed on one of the sensors, retry in 500 ms to confirm */
                if ( data == 0) {
                    usleep(500000); /* 500 MS */

                    cmd = CMD_INVAL;
                    
                    do {
                        if ( send_packet(&UART0, CMD_REFRESH, 0LU) < E_OK ) return E_IOERR;
                        if ( recv_waitfor_packet(&UART0, &cmd, &data, 100) != 0 ) return E_IOERR;
                    } while (cmd == CMD_ERR);

                    /* Error is no longer present, update with new values */
                    if ( cmd == CMD_OK ) {

                        ioexp_update();
                        break;
                    }

                    /* Error presists, change state*/
                    current_state = STATE_HWERR;
                    break;
                }

                /* Temperatures differ, change state */

                break;
            }

            /* Ignore anything else */
            default:
                break;
        }

        update_screen();
    }

    return 0;
}