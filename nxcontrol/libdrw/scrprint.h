#ifndef __SCRPRINT_H__
#define __SCRPRINT_H__

#include "screen.h"

int bind_screen(screen_t* scr);
void unbind_screen(screen_t* scr);
int print(screen_t* scr, const char* str);
void clear_screen(screen_t* scr);

#endif