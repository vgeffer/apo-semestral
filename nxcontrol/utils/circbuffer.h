///@file circbuffer.h
#ifndef __CIRCBUFFER_H__
#define __CIRCBUFFER_H__

#include <stdint.h>

/**
 * Circular buffer 
*/
typedef struct {

    ///Alloc'd size of the buffer
    uint32_t len;
    ///Head index
    uint32_t head; 
    ///Tail index
    uint32_t tail;

    ///Buffer's data
    void** data;

    ///Buffer empty flag
    uint8_t is_empty;

    ///Buffer full flag
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