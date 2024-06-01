///@file scrptint.c
#include "scrprint.h"

#include "drw.h"
#include "font.h"
#include "../common.h"
#include "../utils/circbuffer.h"

#include <stdlib.h>
#include <string.h>

/**
 * Available terminal colors
*/
const uint16_t term_colors[] = {
    ///Unused 
    0,
    ///White
    FROM_RGB(255, 255, 255),
    ///Red
    FROM_RGB(170, 0, 0),
    ///Green
    FROM_RGB(0, 170, 0),
    ///Yellow
    FROM_RGB(170, 85, 0),
    ///Blue
    FROM_RGB(0, 0, 170),
    ///Magenta
    FROM_RGB(170, 0, 170),
    ///Cyan
    FROM_RGB(0, 170, 170),
    ///Ligt Gray
    FROM_RGB(170, 170, 170),
    ///Dark Gray
    FROM_RGB(85, 85, 85),
    ///Light Red
    FROM_RGB(255, 85, 85),
    ///Light Green
    FROM_RGB(85, 255, 85),
    ///Light Yellow
    FROM_RGB(255, 255, 85),
    ///Light Blue
    FROM_RGB(85, 85, 255),
    ///Light Magenta
    FROM_RGB(255, 0, 255),
    ///Light Cyan
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

/**
 * Initialise screen's char buffer
 * @param[in] scr Screen
 * @return E_OK on success, otherwise error
*/
int bind_screen(screen_t* scr) {
    
    return cb_create(scr->lines, SCREEN_HEIGHT / CHAR_HEIGHT);
}

/**
 * Destroy screen's char buffer
 * @param[in] scr Screen
*/
void unbind_screen(screen_t* scr) {

    cb_destroy(scr->lines);
}

/**
 * Print string to the screen. Print recognises several special escape sequences to change the foreground color. The change presists only for one call. 
 * To change the foreground color, isert the sequence '\x1b\cc' into the printed string, where cc is the inde
 * @param[in] scr Screen
 * @param[in] str String to print
 * @return Number of printed characters
*/
int print(screen_t* scr, const char* str) {

    /* If no line is present, create new line */
    if ( scr->lines->is_empty ) 
        if ( create_new_line(scr) < 0 ) return 0;
    
    scr_char_t* line = (scr_char_t*)cb_getlast(scr->lines);
    
    /* Get first non-empty index */
    int start = 0;
    while ( line[start].c != 0 && start < (SCREEN_WIDTH / CHAR_WIDTH) ) start++; 
    
    /* Print out string */
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

/**
 * Clear the sceen's char buffer
 * @param scr Screen
*/
void clear_screen(screen_t* scr) {
    
    while ( !scr->lines->is_empty ) {
        void* item = cb_pop(scr->lines);
        if ( item ) free(item);
    }
}
