///@file spi.h
#ifndef __SPI_H__ 
#define __SPI_H__

#include "../avrdefs.h"

/**
 * Struct containing SPI interface pins
*/
typedef struct { 

    ///Device out - host in
    uint8_t in; 
    ///Host out - device in
    uint8_t out; 
    ///Clock
    uint8_t clk;
} spibus_t;

int spi_init(spibus_t* spi, uint8_t sck, uint8_t miso, uint8_t mosi);
uint8_t spi_get_byte(spibus_t* spi);
void spi_put_byte(spibus_t* spi, uint8_t byte);

#endif

