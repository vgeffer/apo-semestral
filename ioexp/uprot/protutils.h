#ifndef __PROTUTILS_H__
#define __PROTUTILS_H__

#include "protdefs.h"

int make_packet(packet_t* packet, uint16_t cmd, uint64_t data);
int verify_packet(packet_t* packet);

#endif


