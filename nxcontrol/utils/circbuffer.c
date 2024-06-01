///@file circbuffer.c
#include "circbuffer.h"
#include "../common.h"
#include <stdlib.h>

/**
 * Circular buffer constructor
 * @param[in] cb Circ buffer
 * @param[in] len Length to alloc 
 * @return E_OK on success, otherwise error
*/
int cb_create(circ_buffer_t* cb, uint32_t len) {

    if ( !cb ) return E_INVAL;

    cb->len = len;

    cb->head = 0;
    cb->tail = cb->head;

    cb->is_empty = 1;
    cb->is_full = 0;

    if ( (cb->data = (void**)malloc(sizeof(void*) * len)) == NULL ) return E_INVAL;

    return E_OK;
}

/**
 * Circular buffed destructor
 * @param[in] cp Circ buffer
*/
void cb_destroy(circ_buffer_t* cb) {
    
    if ( !cb ) return;

    cb->len = 0;
    free(cb->data);
}

/**
 * Push item at the end of the buffer
 * @param[in] cb Circ buffer
 * @param[in] item Item to push to the list
 * @return 0 if buffer is full, 1 otherwise
*/
int cb_push(circ_buffer_t* cb, void* item) {

    if ( cb->is_full ) return 0;

    /* circ buffer is not full, add item */
    if ( !cb->is_empty )
        cb->tail = (cb->tail + 1) % cb->len;
    cb->data[cb->tail] = item;

    /* Update statuses */
    if ( cb->is_empty ) cb->is_empty = 0;
    if ( cb_length(cb) == cb->len ) cb->is_full = 1;

    return 1;
}

/**
 * Pop item from the start of the buffer
 * @param[in] cb Circ buffer
 * @return NULL if list is empty, otherwise item
*/
void* cb_pop(circ_buffer_t* cb) {

    if ( cb->is_empty ) return NULL;

    /* update statuses */
    if ( cb->is_full ) cb->is_full = 0;
    if ( cb_length(cb) == 1 ) cb->is_empty = 1; 

    /* if not empty, pop item */
    void* item = cb->data[cb->head];
    if ( !cb->is_empty ) cb->head = (cb->head + 1) % cb->len;

    return item;
}

/**
 * Get the i-th item (from the head) of the buffer
 * @param[in] cb Circ buffer 
 * @param[in] i Index
 * @return NULL if index out of bounds, item otherwise
*/
void* cb_get(circ_buffer_t* cb, uint32_t i) {

    if ( cb->is_empty ) return NULL;

    int idx = (cb->head + i) % cb->len;

    /* check for invalid index */
    if ( cb->head < cb->tail && (idx < cb->head || idx > cb->tail) ) return NULL; 
    else if ( cb->head > cb->tail && (idx < cb->head && idx > cb->tail) ) return NULL;
    else if ( cb->head == cb->tail && idx != cb->head ) return NULL;

    return cb->data[idx];
}

/**
 * Get the last item (i = tail) of the buffer
 * @param[in] cb Circ buffer
 * @return NULL if list is empty, otherwise item
*/
void* cb_getlast(circ_buffer_t* cb) {

    if ( cb->is_empty ) return NULL;
    return cb->data[cb->tail];

}

/**
 * Get the length of the buffer
 * @param[in] cb Circ buffer
 * @return Length of the buffer
*/
uint32_t cb_length(circ_buffer_t* cb) {

    if ( cb->is_full ) return cb->len;
    if ( cb->is_empty ) return 0;

    if ( cb->head <= cb->tail ) return (cb->tail - cb->head) + 1;
    else return ((cb->tail + cb->len) - cb->head) + 1;
}
