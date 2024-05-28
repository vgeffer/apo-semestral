#include "dht11.h"
#include "../avrdefs.h"
#include "../interrupts.h"
#include "gpio.h"
#include <stdint.h>
#include <util/delay.h>

#define US_TO_CLOCK(us) ((F_CPU) / 1000000UL) * us

static int await_pulse(dht11_t* dev, uint8_t state) {

    uint16_t count = 0; // To work fast enough on slower AVR boards

    // On AVR platforms use direct GPIO port access as it's much faster and better
    // for catching pulses that are 10's of microseconds in length:
    while ( get_pin_state(dev->pin) == state ) {
        if ( count++ >= dev->timeout )
            return E_TIMEOUT; // Exceeded timeout, fail.
    }

    return count;
}

static int read_val(dht11_t* dev) {

    if ( !dev )
        return E_INVALID;

    /* Dark pointer magic */
    *((uint32_t*)dev->data) = (uint32_t)0;
    
    set_pin_type(dev->pin, PIN_IN);
    set_pin_state(dev->pin, PIN_HI); /* This sets pullup */
    _delay_ms(1);

    // First set data line low for a period according to sensor type
    set_pin_type(dev->pin, PIN_OUT);
    set_pin_state(dev->pin, PIN_LOW);

    _delay_ms(20); // data sheet says at least 18ms, 20ms just to be safe

    int cycles[80];

    /* TIMING CRITICAL */
    disable_intr();
    {
        // End the start signal by setting data line high for 40 microseconds.
        set_pin_type(dev->pin, PIN_IN);
        set_pin_state(dev->pin, PIN_HI); 

        // Delay a moment to let sensor pull data line low.
        _delay_us(55);

        // First expect a low signal for ~80 microseconds followed by a high signal
        // for ~80 microseconds again.
        if (await_pulse(dev, PIN_LOW) < E_OK) {

            enable_intr();
            return E_TIMEOUT;
        }
        if (await_pulse(dev, PIN_HI) < E_OK) {

            enable_intr();
            return E_TIMEOUT;
        }

        for (int i = 0; i < 80; i += 2) {

            cycles[i] = await_pulse(dev, PIN_LOW);
            cycles[i + 1] = await_pulse(dev, PIN_HI);
        }
    }
    enable_intr();

    /* Read the main data */
    for ( int i = 0; i < 32; ++i ) {
    
        uint32_t lowCycles = cycles[2 * i];
        uint32_t highCycles = cycles[2 * i + 1];
    
        if ( (lowCycles == E_TIMEOUT) || (highCycles == E_TIMEOUT) ) 
            return E_TIMEOUT;
      
        dev->data[i / 8] <<= 1;

        // Now compare the low and high cycle times to see if the bit is a 0 or 1.
        if (highCycles > lowCycles)
            dev->data[i / 8] |= 1;
    }

    /* Read the checksum */
    uint8_t chsum = 0;
    for ( int i = 32; i < 40; ++i ) {
    
        uint32_t lowCycles = cycles[2 * i];
        uint32_t highCycles = cycles[2 * i + 1];
    
        if ( (lowCycles == E_TIMEOUT) || (highCycles == E_TIMEOUT) ) 
            return E_TIMEOUT;
      
        chsum <<= 1;

        // Now compare the low and high cycle times to see if the bit is a 0 or 1.
        if (highCycles > lowCycles)
            chsum |= 1;
    }

    // Check we read 40 bits and that the checksum matches.
    if (chsum == ((dev->data[0] + dev->data[1] + dev->data[2] + dev->data[3]) & 0xFF))
        return E_OK;

    return E_INVALID;
}

int dht11_init(dht11_t *dev, uint8_t pin) {

    if ( !dev )
        return E_INVALID;

    dev->pin = pin;
    if ( set_pin_type(pin, PIN_IN) < E_OK )
        return E_OK;

    dev->timeout = US_TO_CLOCK(1000); 
    *((uint32_t*)dev->data) = 0;

    return E_OK;
}

int dht11_read_sensor(dht11_t* dev) {
    
    if ( !dev )
        return E_INVALID;

    int ret = 0;
    if ( (ret = read_val(dev)) < E_OK )
        return ret;

    dev->temp = (dev->data[3] & 0x80 ? (-1.0f - (float)dev->data[2]) : ((float)dev->data[2])) + ((float)(dev->data[3] & 0x0F) * 0.1f); 
    dev->humid = (float)(dev->data[0] + dev->data[1]) * 0.1f;

    return E_OK;
}