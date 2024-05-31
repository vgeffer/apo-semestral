#include "protutils.h"
#include "protdefs.h"
#include "../common.h"

#include <stdint.h>
#include <stddef.h>
#include <sys/poll.h>

static packet_t last_sent = { .command = CMD_INVAL };

static uint16_t crc16(uint8_t* data, uint16_t size) {

    uint8_t x;
    uint16_t crc = 0x1D0F;

    while ( size-- ){
        x = crc >> 8 ^ *data++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    return crc;
}

static int make_packet(packet_t* packet, uint16_t command, uint64_t data) {

    if ( packet == NULL ) 
        return E_INVAL;

    packet->head = PACKET_HEAD;

    packet->command = command;
    packet->data = data;
    
    packet->tail = PACKET_TAIL;
    
    packet->crc = crc16((uint8_t*)packet, (uint16_t)(sizeof(packet_t) - sizeof(uint16_t)));

    return E_OK;
}

static int verify_packet(packet_t *packet) {


    if ( packet == NULL )
        return E_INVAL;

    /* Check head & tail */
    if ( packet->head != (PACKET_HEAD) || packet->tail != (PACKET_TAIL) )
        return E_INVAL;

    uint16_t crc = crc16((uint8_t*)packet, (uint16_t)(sizeof(packet_t) - sizeof(uint16_t)));
    if ( packet->crc != crc )
        return E_INVAL;

    return E_OK;
}

int await_recv(uart_t* port, int max_timeout) {

    if ( !port ) return E_INVAL;

    struct pollfd pfd = {
        .fd = port->uart_fd,
        .events = POLLIN | POLLPRI
    };

    int ret;
    if ( (ret = poll(&pfd, 1, max_timeout)) < 0 ) return E_INVAL;

    if ( ret == 0 ) return E_TIMEOUT; 

    return E_OK; /* Data ready to be read */
}

int send_packet(uart_t* port, uint16_t cmd, uint64_t data) {

    union {
        packet_t packet;
        uint8_t data[sizeof(packet_t)];
    } uprot_packet;


    if ( make_packet(&uprot_packet.packet, cmd, data) < 0 ) return E_INVAL;

    for ( uint8_t i = 0; i < sizeof(packet_t); i++ ) {

        uart_putc(port, uprot_packet.data[i]);
    } 

    last_sent = uprot_packet.packet;
    return E_OK;
}

int send_retry_last(uart_t* port) {

    if ( last_sent.command == CMD_INVAL )
        return E_INVAL;

    union {
        packet_t packet;
        uint8_t data[sizeof(packet_t)];
    } uprot_packet = { .packet = last_sent };

    for ( uint8_t i = 0; i < sizeof(packet_t); i++ ) {

        uart_putc(port, uprot_packet.data[i]);
    } 

    return E_OK;
}


int recv_packet(uart_t* port, uint16_t* cmd, uint64_t* data) {

    if ( cmd == NULL || data == NULL )
        return E_INVAL;

    union {
        packet_t packet;
        uint8_t data[sizeof(packet_t)];
    } uprot_packet;


    for ( uint8_t i = 0; i < sizeof(packet_t); i++ ) {

        uprot_packet.data[i] = (uint8_t)uart_getc(port);
    } 

    if ( verify_packet(&uprot_packet.packet) < 0 ) {
     
        return E_INVAL;
    }

    *cmd = uprot_packet.packet.command;
    *data = uprot_packet.packet.data;

    return E_OK;
}

/* Just calling await and recv but in a more conveninet package! */
int recv_waitfor_packet(uart_t* port, uint16_t* cmd, uint64_t* data, int max_timeout) {

    int ret = await_recv(port, max_timeout);
    if ( ret != 0 ) return ret;

    return recv_packet(port, cmd, data);
}