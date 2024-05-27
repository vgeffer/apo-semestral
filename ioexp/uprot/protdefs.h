
/* Frame protocol for ESP <-> IOEXP communication */
/* Based on UART */

#ifndef __PROTDEFS_H__
#define __PROTDEFS_H__

#include <stdint.h>

#define UPROT_BAUD 9600

#define TO_WORD(a, b) a << 8 | b

#define PACKET_HEAD TO_WORD('S', 'P')
#define PACKET_TAIL TO_WORD('E', 'P')

enum valid_commands {
    
    /* Status commands */
    CMD_OK,                 /* Returning resolved data */
    CMD_ERR,                /* Error */

    /* Functional requests (-> arduino)*/
    CMD_TEMP,
    CMD_HUMID,
    CMD_REFRESH,
    
    /* Notifications (<- arduino) */
    CMD_HW_ERR,
    CMD_EMERGENCY,

    CMD_INVAL = 0xFFFF      /* Invalid command, to be used as an invalid state */
};

typedef struct  {

    uint16_t head;
    
    uint16_t command;
    uint64_t data;

    uint16_t tail;
    uint16_t crc;

} packet_t;


#endif