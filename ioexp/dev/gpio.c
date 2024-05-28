#include "gpio.h"

enum PORTS {
    PORT_B, PORT_C, PORT_D
};

struct  {
    uint8_t port;
    uint8_t bit;
} static const board_pin_map[] = {
    {PORT_D, PORTD0},
    {PORT_D, PORTD1},
    {PORT_D, PORTD2},
    {PORT_D, PORTD3},
    {PORT_D, PORTD4},
    {PORT_D, PORTD5},
    {PORT_D, PORTD6},
    {PORT_D, PORTD7},
    
    {PORT_B, PORTB0},
    {PORT_B, PORTB1},
    {PORT_B, PORTB2},
    {PORT_B, PORTB3},
    {PORT_B, PORTB4},
    {PORT_B, PORTB5},

    {PORT_C, PORTC0},
    {PORT_C, PORTC1},
    {PORT_C, PORTC2},
    {PORT_C, PORTC3},
    {PORT_C, PORTC4},
    {PORT_C, PORTC5},
};

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
