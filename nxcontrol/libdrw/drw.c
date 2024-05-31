#include "drw.h"
#include "font.h"
#include "screen.h"

#define SCR_CHAR_WIDTH SCREEN_WIDTH / CHAR_WIDTH
#define SCR_CHAR_HEIGHT SCREEN_HEIGHT / CHAR_HEIGHT

static void set_drawing_line(screen_t* scr, uint16_t line) {

    if ( line > SCREEN_HEIGHT ) return;

    scr_enable(scr);

    scr_command(scr, COADDR);   
    scr_data(scr, 0);               /* X start */
    scr_data(scr, SCREEN_WIDTH);    /* X end */

    scr_command(scr, PGADDR);
    scr_data(scr, line);     /* Y start */
    scr_data(scr, line + 1);     /* Y end */

    scr_disable(scr);
}

void draw(screen_t* scr) {

    if ( !scr ) return;

    scr_enable(scr);

    int y = 0;
    for ( ; y < cb_length(scr->lines); y++ ) {

        scr_char_t* line = cb_get(scr->lines, y);

        for ( int y_c = 0; y_c < CHAR_HEIGHT - 1; y_c++ ) {

            uint8_t draw_end = 0;
            set_drawing_line(scr, y * CHAR_HEIGHT + y_c);

            /* Begin drawing line */
            scr_command(scr, DMWR);

            for ( int x = 0; x < SCR_CHAR_WIDTH; x++ ) {

                char c;
                if ( draw_end ) c = ' ';
                else c = line[x].c;
                
                if ( !c ) {
                    
                    draw_end = 1;
                    c = ' ';
                }

                uint8_t char_row = font[c - 32][y_c]; 
                uint16_t color = line[x].col;

                for (int x_c = 0; x_c < CHAR_WIDTH; x_c++) {

                    uint16_t pix = (((char_row << x_c) & 0x80) >> 7) * color;
                    scr_data(scr, pix);
                }
            }
        }

        /* Begin drawing line */ 
        set_drawing_line(scr, y * CHAR_HEIGHT + (CHAR_HEIGHT - 1));
        scr_command(scr, DMWR);
        
        for ( int x = 0; x < SCREEN_WIDTH; x++ )
            scr_data(scr, scr->bg_color);
    }

    /* Fill the rest of the screen with bg */
    y *= CHAR_HEIGHT; /* Convert from rows of chars to rows of px */
    for ( ; y < SCREEN_HEIGHT; y++ ) {

        /* Begin drawing line */
        set_drawing_line(scr, y * CHAR_HEIGHT);
        scr_command(scr, DMWR);

        for ( int x = 0; x < SCREEN_WIDTH; x++ )
            scr_data(scr, scr->bg_color);
    }

    scr_disable(scr);
}