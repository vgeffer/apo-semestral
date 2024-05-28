#ifndef __SPI_H__ 
#define __SPI_H__

#include "../avrdefs.h"

typedef struct { 
    uint8_t in, out, clk;
} spibus_t;

int spi_init(spibus_t* spi, uint8_t sck, uint8_t miso, uint8_t mosi);
uint8_t spi_get_byte(spibus_t* spi);
void spi_put_byte(spibus_t* spi, uint8_t byte);

#endif

