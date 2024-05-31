#include "shiftreg.h"
#include "gpio.h"

#include <unistd.h>

void sh_writeb(shiftreg_t* sr, uint8_t data) {

    if (!sr) return;

    for (int i = 0; i < sizeof(data) * 8; i++) {
        uint8_t bit = (data >> i) & 1;
        
        set_pin(sr->p_sda, bit);

        /* Pulse clock */
        usleep(100); /* Wait for data to stabilise */
        set_pin(sr->p_clk, PIN_HI);
        usleep(200); /* Wait for data to be taken in */
        set_pin(sr->p_clk, PIN_LOW);
    }
}

void sh_writew(shiftreg_t* sr, uint16_t data) {

    if (!sr) return;

    for (int i = 0; i < sizeof(data) * 8; i++) {
        uint8_t bit = (data >> i) & 1;
        
        set_pin(sr->p_sda, bit);

        /* Pulse clock */
        usleep(100); /* Wait for data to stabilise */
        set_pin(sr->p_clk, PIN_HI);
        usleep(200); /* Wait for data to be taken in */
        set_pin(sr->p_clk, PIN_LOW);
    }

}

void sh_writedw(shiftreg_t* sr, uint32_t data) {

    if (!sr) return;

    for (int i = 0; i < sizeof(data) * 8; i++) {
        uint8_t bit = (data >> i) & 1;
        
        set_pin(sr->p_sda, bit);

        /* Pulse clock */
        usleep(1); /* Wait for data to stabilise */
        set_pin(sr->p_clk, PIN_HI);
        usleep(2); /* Wait for data to be taken in */
        set_pin(sr->p_clk, PIN_LOW);
    }
}