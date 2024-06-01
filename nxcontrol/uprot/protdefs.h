
/* Frame protocol for ESP <-> IOEXP communication */
/* Based on UART */

#ifndef __PROTDEFS_H__
#define __PROTDEFS_H__

#include <stdint.h>
#include <termios.h>

#define UPROT_BAUD B9600

#define TO_WORD(a, b) a << 8 | b

#define PACKET_HEAD TO_WORD('S', 'P')
#define PACKET_TAIL TO_WORD('E', 'P')

enum valid_commands {
    
    /* Status commands */
    CMD_OK,                 
    CMD_ERR,                


    /* Functional requests (-> arduino)*/
    CMD_TEMP,
    CMD_HUMID,
    CMD_REFRESH,
    
    /* Notifications (<- arduino) */
    CMD_HW_ERR,
    CMD_EMERGENCY,

    CMD_INVAL = 0x7FFF      /* Invalid command, to be used as an invalid state */
};

/**
 * Structure of the packet sent between IOExpander and ESP32 controller
*/
typedef struct  {

    ///Packet header (letters SP)
    uint16_t head;
    ///Packet's command
    uint16_t command;
    ///Packet's payload
    uint64_t data;
    ///Packet tail (letters EP)
    uint16_t tail;
    ///CRC16 checksum
    uint16_t crc;

} packet_t;


#endif