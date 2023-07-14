#include <stdio.h>
#include <string.h>
#include "display.h"

void display_init(display *display){
    memset(display->screen, 0, sizeof(display->screen));
    display->scanline_counter = SCALINE_COUNTER_INIT_VAL;
}

bool is_LCD_enabled(gb_memory *memory){
    return (read_one_byte(memory, LCD_CTRL_REG_ADDR) & 0b10000000);
}