// The screen resolution is 160 x 144, meaning tha there are 144 visible scanlines (horizontal lines). The gb draws
// each scanline one at a time starting from 0 to 153, meaning that there are an extra 8 scanlines that are invisible
// when the current scanline is between 144 and 153, this is the vertical blank period. The current scaline is stored in 
// memory register at address 0xFF44. As per https://gbdev.io/pandocs/, it takes 456 cpu clock cycles to draw one scanline 
// and move onto the next, so we will need a counter to know when to move onto the next line. Similar to timer counter 
// and divider register counter, we control this counter by subtracting its value by the amount of clock cycles the last 
// opcode took to execute.

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define SCALINE_CLK_COUNTER_INIT_VAL 456
#define LCD_CTRL_REG_ADDR 0xFF40
#define LCD_STATUS_REG 0xFF41
#define LCD_SCROLL_Y 0xFF42 // The Y Position of the BACKGROUND where to start drawing the viewing area from
#define LCD_SCROLL_X 0xFF43 // The X Position of the BACKGROUND to start drawing the viewing area from
#define LCD_WINDOW_Y 0xFF4A // The Y Position of the VIEWING AREA to start drawing the window from
#define LCD_WINDOW_X 0xFF4B // The X Positions -7 of the VIEWING AREA to start drawing the window from
#define LCD_Y_COORDINATE_REG 0xFF44
#define LCD_Y_COMPARE_REG 0xFF45

typedef struct display{
    // x. y, rbg value -> 0 = red, 1 = green, 2 = blue
    uint8_t screen[160][144][3];

    // it takes 456 cycles to draw a scanline
    int scanline_counter;
} display;

void display_init(display *display);






#endif