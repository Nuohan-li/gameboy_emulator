// The screen resolution is 160 x 144, meaning tha there are 144 visible scanlines (horizontal lines). The gb draws
// each scanline one at a time starting from 0 to 153, meaning that there are an extra 8 scanlines that are invisible
// when the current scanline is between 144 and 153, this is the vertical blank period. The current scaline is stored in 
// memory register at address 0xFF44. As per https://gbdev.io/pandocs/, it takes 456 cpu clock cycles to draw one scanline 
// and move onto the next, so we will need a counter to know when to move onto the next line. Similar to timer counter 
// and divider register counter, we control this counter by subtracting its value by the amount of clock cycles the last 
// opcode took to execute.

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

#define SCALINE_COUNTER_INIT_VAL 456
#define LCD_CTRL_REG_ADDR 0xFF40

typedef struct display{
    // x. y, rbg value -> 0 = red, 1 = green, 2 = blue
    uint8_t screen[160][144][3];
    int scanline_counter;
}display;

void display_init(display *display);
void update_graphics(display *display, int cycles);

// LCD control
bool is_LCD_enabled(gb_memory *memory);