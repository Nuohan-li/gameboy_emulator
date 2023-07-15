#include <stdio.h>
#include <string.h>
#include "display.h"
#include "Interrupt.h"

void display_init(display *display){
    memset(display->screen, 0, sizeof(display->screen));
    display->scanline_counter = SCALINE_CLK_COUNTER_INIT_VAL;
}

