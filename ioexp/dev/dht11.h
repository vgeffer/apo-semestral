/* 
    This is a port of a Adafruit DHT11 library
    https://github.com/adafruit/DHT-sensor-library/  

    Copyright (c) 2020 Adafruit Industries


    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
    OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file dht11.h
 * For documentation refer to https://github.com/adafruit/DHT-sensor-library
*/
#ifndef __DHT_11__
#define __DHT_11__

#include "../avrdefs.h"

/**
 * Structure containing the state of the DHT11 driver
*/
typedef struct {

    ///I/O pin
    uint8_t pin;
    ///Internal data buffer
    uint8_t data[4];
    ///Maximum timeout to wait for a pulse
    uint32_t timeout; /* In cycles */

    ///Last read temperature
    float temp;
    ///Last read humidity
    float humid;

} dht11_t;

int dht11_init(dht11_t* dev, uint8_t pin);
int dht11_read_sensor(dht11_t* dev);

#endif
