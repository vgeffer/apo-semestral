#include "scrprint.h"

#include "drw.h"
#include "font.h"
#include "../common.h"
#include "../utils/circbuffer.h"

#include <stdlib.h>
#include <string.h>


const uint16_t term_colors[] = {
    0,
    FROM_RGB(255, 255, 255),
    FROM_RGB(170, 0, 0),
    FROM_RGB(0, 170, 0),
    FROM_RGB(170, 85, 0),
    FROM_RGB(0, 0, 170),
    FROM_RGB(170, 0, 170),
    FROM_RGB(0, 170, 170),
    FROM_RGB(170, 170, 170),
    FROM_RGB(85, 85, 85),
    FROM_RGB(255, 85, 85),
    FROM_RGB(85, 255, 85),
    FROM_RGB(255, 255, 85),
    FROM_RGB(85, 85, 255),
    FROM_RGB(255, 0, 255),
    FROM_RGB(0, 255, 255)

    
};

static int create_new_line(screen_t* scr) {
    scr_char_t* line = (scr_char_t*)malloc(sizeof(scr_char_t) * (SCREEN_WIDTH / CHAR_WIDTH));
    if ( !line ) return E_INVAL;
    
    memset(line, 0, sizeof(scr_char_t) * (SCREEN_WIDTH / CHAR_WIDTH));

    /* Scroll up if not enough space left */
    if ( !cb_push(scr->lines, line) ) {
        void* item = cb_pop(scr->lines);
        cb_push(scr->lines, line);

        if ( item ) free(item);
    }
    return E_OK;
}

int bind_screen(screen_t* scr) {
    
    return cb_create(scr->lines, SCREEN_HEIGHT / CHAR_HEIGHT);
}

void unbind_screen(screen_t* scr) {
    cb_destroy(scr->lines);
}

int print(screen_t* scr, const char* str) {

    if ( scr->lines->is_empty ) {
        if ( create_new_line(scr) < 0 ) return 0;
    }
    scr_char_t* line = (scr_char_t*)cb_getlast(scr->lines);
    
    int start = 0;
    while ( line[start].c != 0 && start < (SCREEN_WIDTH / CHAR_WIDTH) ) start++; /* Get first non-empty index */
    
    int i = 0;
    uint16_t current_fg = term_colors[1];
    while ( str[i] != '\00' ) {

        /* Current line is full, start a new one */
        if ( start == (SCREEN_WIDTH / CHAR_WIDTH) ) { 

            if ( create_new_line(scr) < 0 ) return 0;
            line = (scr_char_t*)cb_getlast(scr->lines);
            start = 0;
        }

        /* Process special chars */
        switch (str[i]) {

            /* 
              Escape sequences - only supports color. These are not ANSI escape sequences! 
              They use special format: \x1b\cc where cc denotes an index byte into color table
              For safety reasons, color 00 is treated as end of string.
            */
            case '\x1b': {
                
                uint8_t color = str[i + 1];
                if ( color == 0 ) {
                    /* Loop will end on next iter */
                    i += 1;
                    continue;
                }
            
                /* Safety check so we don't overflow the array */
                if ( color >= (sizeof(term_colors) / sizeof(term_colors[0])) ) color = 1; 

                current_fg = term_colors[color];

                i += 2;
                continue;
            }

            case '\n': {

                line[start].c = '\00';
                
                if ( create_new_line(scr) < 0 ) return i; /* I chars already printed */
                line = (scr_char_t*)cb_getlast(scr->lines); /* Start new line */
                
                start = 0;
                i += 1;
                continue;
            }
        }

        /* Skip any non-printable char */
        if ( str[i] < ' ' ) continue;

        line[start].c = str[i];
        line[start].col = current_fg;

        i += 1;
        start += 1;
    }

    return i;
}

void clear_screen(screen_t* scr) {
    
    while ( !scr->lines->is_empty ) {
        void* item = cb_pop(scr->lines);
        if ( item ) free(item);
    }
}
