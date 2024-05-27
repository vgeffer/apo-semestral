#include <stdio.h>

#include "uart.h"
#include "../interrupts.h"
#include "../uprot/protdefs.h"
#include "../uprot/protutils.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/setbaud.h>

static int uart_putc(char c, FILE* str);
static int uart_getc(FILE* str);

static FILE uart_fp = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);
static packet_t last_packet = { .command = 0xFFFF };


int uart_init(void) {

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);

    return E_OK;
}

int uart_transmit(uint16_t cmd, uint64_t buf, uint8_t flags) {

    /* Prepare packet */    
    union {
        packet_t packet;
        uint8_t data[sizeof(packet_t)];
    } uprot_packet;

    if ( flags & F_RETRY ) {

        if ( last_packet.command == CMD_INVAL )
            return E_INVALID;

        uprot_packet.packet = last_packet;
    }

    else if ( make_packet(&uprot_packet.packet, cmd, buf) < E_OK )
        return E_INVALID;

    if ( flags & F_NOINTR )
        disable_intr();

    for ( uint8_t i = 0; i < sizeof(packet_t); i++ ) {

        /* Check if the transfer was interrupted */
        if ( !(flags & F_NOINTR) && get_intr_flag() )
            return E_INTR;

        putc(uprot_packet.data[i], &uart_fp);
    } 

    if ( flags & F_NOINTR )
        enable_intr();

    last_packet = uprot_packet.packet;
    return E_OK;
}

int uart_recv(uint16_t *cmd, uint64_t *buf, uint8_t flags) {

    if ( cmd == NULL || buf == NULL )
        return E_INVALID;

    if ( flags & F_NOINTR )
        disable_intr();

    union {
        packet_t packet;
        uint8_t data[sizeof(packet_t)];
    } uprot_packet;


    for ( uint8_t i = 0; i < sizeof(packet_t); i++ ) {

        /* Check if the transfer was interrupted */
        if ( !(flags & F_NOINTR) && get_intr_flag() )
            return E_INTR;

        uprot_packet.data[i] = (uint8_t)getc(&uart_fp);
    } 

    if ( verify_packet(&uprot_packet.packet) < E_OK ) {
     
        if ( flags & F_NOINTR )
            enable_intr();

        return E_INVALID;
    }

    *cmd = uprot_packet.packet.command;
    *buf = uprot_packet.packet.data;
    
    if ( flags & F_NOINTR )
        enable_intr();

    return E_OK;
}

static int uart_putc(char c, FILE* stream) {
    
    loop_until_bit_is_set(UCSR0A, TXC0);
    UDR0 = c;

    return E_OK;
}

static int uart_getc(FILE* stream) {

    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}
