///@file interrupts.c
#include "interrupts.h"
#include "dev/gpio.h"

static uint8_t intr_flag;
static void (*usart_rx_intr_func)(void) = NULL; 

/**
 * Check wether interrupt has been triggered
*/
uint8_t get_intr_flag(void) {

    uint8_t flag = intr_flag;
    intr_flag = 0;

    return flag;
}

/**
 * Set the interrupt callback
 * @param[in] intr_fnc Interrupt function
*/
void set_intr_func(void (*intr_fnc)(void)) {
    usart_rx_intr_func = intr_fnc;
}


/* List of interrupts */
static ISR(USART_RX_vect) {
    
    set_pin_state(LED_BUILTIN, PIN_HI);
    intr_flag = 1;

    if ( usart_rx_intr_func )
        usart_rx_intr_func();

    set_pin_state(LED_BUILTIN, PIN_LOW);
}
