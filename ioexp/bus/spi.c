#include "spi.h"
#include "../dev/gpio.h"
#include <stdint.h>
#include <util/delay.h>

int spi_init(spibus_t* spi, uint8_t sck, uint8_t miso, uint8_t mosi) {

    if ( !spi ) 
        return -1;

    spi->clk = sck;
    spi->in = miso;
    spi->out = mosi;

    if ( set_pin_type(sck, PIN_OUT) < 0 ) 
        return -1;

    if ( set_pin_type(miso, PIN_IN) < 0 )
        return -1;

    if ( set_pin_type(mosi, PIN_OUT) < 0 )
        return -1;
    
    return 0;
}

uint8_t spi_get_byte(spibus_t* spi) {

    uint8_t byte = 0;

    set_pin_state(spi->clk, PIN_LOW);
    _delay_us(10);

    for (int i = 7; i >= 0; i--) {

        set_pin_state(spi->clk, PIN_HI);
        _delay_us(10);

        int bit = get_pin_state(spi->in);
        if ( bit < 0 ) 
            return -1; 

        byte |= ((uint8_t)bit & 1) << i; /* I believe this should be different but shure */

        set_pin_state(spi->clk, PIN_LOW);
        _delay_us(10);
    }
    
    set_pin_state(spi->clk, PIN_HI);
    return byte;
}

void spi_put_byte(spibus_t* spi, uint8_t byte) {

    /* Prepare for transmission */
    set_pin_state(spi->clk, PIN_LOW);
    _delay_us(10);

    for ( int i = 0; i < 8; i++ ) {

        set_pin_state(spi->clk, PIN_HI);
        _delay_us(10);

        uint8_t bit = (byte >> i) & 1;
        set_pin_state(spi->out, bit);

        set_pin_state(spi->clk, PIN_LOW);
        _delay_us(10);
        
    }

    /* Set to idle state */
    set_pin_state(spi->clk, PIN_HI);
}