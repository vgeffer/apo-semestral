///@file protutils.c
#include "protutils.h"
#include "protdefs.h"
#include "../avrdefs.h"

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

/**
 * Create uprot packet from command no and data buffer
 * @param[out] packet Created packet
 * @param[in] command Packet's command
 * @param[in] data Packet's payload
 * @return E_OK on success, otherwise error
*/
int make_packet(packet_t* packet, uint16_t command, uint64_t data) {

    if ( packet == NULL )
        return E_NULLPTR;

    packet->head = PACKET_HEAD;

    packet->command = command;
    packet->data = data;
    
    packet->tail = PACKET_TAIL;
    
    packet->crc = crc16((uint8_t*)packet, (uint16_t)(sizeof(packet_t) - sizeof(uint16_t)));

    return E_OK;
}

/**
 * Verify received packet's integrity
 * @param[in] packet Packet to verify
 * @return E_OK on success, error otherwise
*/
int verify_packet(packet_t *packet) {


    if ( packet == NULL )
        return E_NULLPTR;

    /* Check head & tail */
    if ( packet->head != (PACKET_HEAD) || packet->tail != (PACKET_TAIL) )
        return E_INVALID;

    uint16_t crc = crc16((uint8_t*)packet, (uint16_t)(sizeof(packet_t) - sizeof(uint16_t)));
    if ( packet->crc != crc )
        return E_INVALID;

    return E_OK;
}