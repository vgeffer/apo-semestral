#ifndef __SHIFTREG_H__
#define __SHIFTREG_H__

#include <stdint.h>

typedef struct {

    uint8_t p_clk;
    uint8_t p_sda;
    uint8_t p_clr;

} shiftreg_t;

void sh_writeb(shiftreg_t* reg, uint8_t data);
void sh_writew(shiftreg_t* reg, uint16_t data);
void sh_writedw(shiftreg_t* reg, uint32_t data);

#endif