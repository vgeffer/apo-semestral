#ifndef __CIRCBUFFER_H__
#define __CIRCBUFFER_H__

#include <stdint.h>

typedef struct {

    uint32_t len;
    uint32_t head, tail;

    void** data;

    uint8_t is_empty;
    uint8_t is_full;

} circ_buffer_t;

int cb_create(circ_buffer_t* cb, uint32_t len);
void cb_destroy(circ_buffer_t* cb);
int cb_push(circ_buffer_t* cb, void* item);
void* cb_pop(circ_buffer_t* cb);
void* cb_get(circ_buffer_t* cb, uint32_t i);
void* cb_getlast(circ_buffer_t* cb);
uint32_t cb_length(circ_buffer_t* cb);


#endif