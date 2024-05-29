#include "avrdefs.h"
#include "bus/spi.h"
#include "bus/uart.h"

#include "dev/gpio.h"
#include "dev/dht11.h"
#include "dev/max6675.h"
#include "interrupts.h"
#include "uprot/protdefs.h"

#include <math.h>
#include <stdint.h>
#include <avr/sleep.h>
#include <util/delay.h>

/* If measured temperature is higher than this, automatically trigger emergency */
const float EMERGENCY_TEMP_TRESHOLD = 100.0f;

max6675_t temp_sensor;
dht11_t humidity_sensor;
spibus_t spi;

void uart_intr_func(void) {

    uint16_t cmd;
    uint64_t buf;

    if ( uart_recv(&cmd, &buf, F_NOINTR) < E_OK )
        uart_transmit(CMD_ERR, 0UL, F_NOINTR);

    switch ( cmd ) {

        case CMD_ERR: {
            
            /* Re-transmit last packet, data and cmd are irrelevant here */
            uart_transmit(0, 0, F_RETRY | F_NOINTR);
            break;
        }

        /* Retrieve Temp*/
        case CMD_TEMP: {

            uint32_t* data_low = ((uint32_t*)&temp_sensor.temp);
            uart_transmit(CMD_OK, 0UL | *data_low , F_NOINTR);
            break;
        }

        /* Retrieve humidity */
        case CMD_HUMID: {

            uint32_t* data_low = ((uint32_t*)&humidity_sensor.humid);
            uart_transmit(CMD_OK, 0UL | *data_low , F_NOINTR);
            break;
        }

        /* Force refresh is requested */
        case CMD_REFRESH: {
            
            if ( max6675_read_temp(&temp_sensor, &spi) < E_OK || dht11_read_sensor(&humidity_sensor) < E_OK ) 
                uart_transmit(CMD_HW_ERR, 0UL, F_NOINTR);
            
            else
                uart_transmit(CMD_OK, 0UL, F_NOINTR);

            break;
        }

        /* Ignore */
        default:
            break;
    }
}

void fatal_err(void) {
    
    char f = 1; 
    while( 1 ) { 
        
        set_pin_state(LED_BUILTIN, f); 
        f = f ^ 1; 
        _delay_ms(500); 
    } 
}

int main(void) {

    set_pin_type(LED_BUILTIN, PIN_OUT);
    set_pin_state(LED_BUILTIN, PIN_LOW);

    /* Init buses */
    if (uart_init() < E_OK)
        fatal_err();

    if (spi_init(&spi, 10, 8, 9) < E_OK)
        fatal_err();

    /* Init devices */
    if (max6675_init(&temp_sensor, 11) < E_OK)
        fatal_err();

    if (dht11_init(&humidity_sensor, 12) < E_OK)
        fatal_err();

    set_intr_func(&uart_intr_func);

    float prev_temp = NAN;
    /* Main loop */
    while ( 1 ) {


        /* Poll data every 2 seconds */
        _delay_ms(2000);

        /* Update sensors */
        if ( max6675_read_temp(&temp_sensor, &spi) < E_OK || dht11_read_sensor(&humidity_sensor) < E_OK ) {
        
            uart_transmit(CMD_HW_ERR, 0UL, F_NOINTR);
            continue;
        }
        
        /* Check for temperature variation */
        /* If temps differ by more than 5C, we may assume one of the sensors is faulty */
        if ( fabs(temp_sensor.temp - humidity_sensor.temp) > 5.0f ) {

            /* Upper 32B -> humidity sensor's temp */
            /* Lower 32B -> temperature sensor's temp */
            union {
                struct {
                    float hs_temp;
                    float ts_temp;
                } __attribute__((packed)) temps;
                uint64_t buffer;
            } msg = { .temps.ts_temp = temp_sensor.temp, .temps.hs_temp = humidity_sensor.temp };

            uart_transmit(CMD_HW_ERR, msg.buffer, F_NOINTR);

            continue;
        }

        /* Check for rapid rise of temperature */
        /* If temperature rose more than 20C in last 2 seconds, somethin's burnin' */
        if ( prev_temp != NAN && (temp_sensor.temp - prev_temp) > 20.0f ) {

            uart_transmit(CMD_EMERGENCY, 0UL, F_NOINTR);
            continue;
        }

        /* Check if temperature has risen above certain treshold */
        if ( temp_sensor.temp > EMERGENCY_TEMP_TRESHOLD ) {

            uart_transmit(CMD_EMERGENCY, 0UL, F_NOINTR);
            continue;
        }

        prev_temp = temp_sensor.temp;
    }
}