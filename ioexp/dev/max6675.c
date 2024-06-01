#include "max6675.h"
#include "gpio.h"

#include <math.h>
#include <util/delay.h>

/**
 * Set up the MAX6675 device
 * @param[in] m MAX6675 device struct
 * @param[in] cs Number of the MAX6675's chip select pin
 * @return E_OK on success, otherwise error
*/
int max6675_init(max6675_t* m, uint8_t cs) {

    if (m == NULL)
        return E_NULLPTR;

    if ( set_pin_type(cs, PIN_OUT) < E_OK )
        return E_INVALID;

    if ( set_pin_state(cs, PIN_HI) < E_OK )
        return E_INVALID;

    m->cs_pin = cs;
    m->temp = NAN;

    return E_OK;
}

/**
 * Read the temperature from sensor
 * @param[in] m MAX6675 device struct
 * @param[in] spi SPI bus struct
 * @return E_OK on success, error otherwise
*/
int max6675_read_temp(max6675_t* m, spibus_t* spi) {

    uint16_t temp;

    set_pin_state(m->cs_pin, PIN_LOW);
    _delay_us(10);

    temp = spi_get_byte(spi);
    temp <<= 8;
    temp |= spi_get_byte(spi);

    set_pin_state(m->cs_pin, PIN_HI);

    if ( temp & 0x4 ) 
        return E_INVALID;
    
    temp >>= 3; 

    m->temp = (float)temp * 0.25f;
    return E_OK;
}
