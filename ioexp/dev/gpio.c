///@file gpio.c
#include "gpio.h"

///List of constants representing ATmega's GPIO ports
enum PORTS {
    PORT_B, PORT_C, PORT_D
};

struct  {
    uint8_t port;
    uint8_t bit;
} static const board_pin_map[] = {
    {PORT_D, 0},
    {PORT_D, 1},
    {PORT_D, 2},
    {PORT_D, 3},
    {PORT_D, 4},
    {PORT_D, 5},
    {PORT_D, 6},
    {PORT_D, 7},
    
    {PORT_B, 0},
    {PORT_B, 1},
    {PORT_B, 2},
    {PORT_B, 3},
    {PORT_B, 4},
    {PORT_B, 5},

    {PORT_C, 0},
    {PORT_C, 1},
    {PORT_C, 2},
    {PORT_C, 3},
    {PORT_C, 4},
    {PORT_C, 5},
};

/**
 * Set the type of the pin
 * @param[in] pin Pin number
 * @param[in] state New state of the pin (PIN_IN / PIN_OUT)
 * @return E_OK on success, otherwise error
*/
int set_pin_type(uint8_t pin, uint8_t state) {

    /* Set output to low, change then set output back*/

    if ( pin >= (sizeof(board_pin_map) / sizeof(board_pin_map[0])) )
        return -1;

    switch ( board_pin_map[pin].port ) {

        case PORT_B:
            DDRB = (state & 1) << board_pin_map[pin].bit;
            break;
        
        case PORT_C:
            DDRC = (state & 1) << board_pin_map[pin].bit;
            break;

        case PORT_D:
            DDRD = (state & 1) << board_pin_map[pin].bit;
            break;
    }

    return 0;
}

/**
 * Set the logical state of the pin
 * @param[in] pin Pin number
 * @param[in] state New logic state of the pin (PIN_LOW / PIN_HI)
 * @return E_OK on success, otherwise error
*/
int set_pin_state(uint8_t pin, uint8_t state) {

    if ( pin >= (sizeof(board_pin_map) / sizeof(board_pin_map[0])) )
        return -1;

    switch ( board_pin_map[pin].port ) {

        case PORT_B:
            PORTB = (state & 1) << board_pin_map[pin].bit;
            break;
        
        case PORT_C:
            PORTC = (state & 1) << board_pin_map[pin].bit;
            break;

        case PORT_D:
            PORTD = (state & 1) << board_pin_map[pin].bit;
            break;
    }

    return 0;
}

/**
 * Read the state of the pin
 * @param[in] pin Pin number
 * @return Pin's logic state (PIN_LOW / PIN_HI) on success, otherwise error
*/
int get_pin_state(uint8_t pin) {

    if ( pin >= (sizeof(board_pin_map) / sizeof(board_pin_map[0])) )
        return -1;
    
    switch ( board_pin_map[pin].port ) {
        
        case PORT_B:
            return (int)((PINB >> board_pin_map[pin].bit) & 1);

        case PORT_C:
            return (int)((PINC >> board_pin_map[pin].bit) & 1);
        
        case PORT_D:
            return (int)((PIND >> board_pin_map[pin].bit) & 1);

        default:
            return -1;
    }
}
