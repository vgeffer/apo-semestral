#ifndef __MAX6675_H__
#define __MAX6675_H__

#include "../avrdefs.h"
#include "../bus/spi.h"

typedef struct {

    uint8_t cs_pin;
    float temp;
} max6675_t;

int max6675_init(max6675_t* m, uint8_t cs);
int max6675_read_temp(max6675_t* m, spibus_t* spi);

#endif