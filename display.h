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
#include "cpu.h"

#define SCALINE_CLK_COUNTER_INIT_VAL 456
#define LCD_CTRL_REG_ADDR 0xFF40
#define LCD_STATUS_REG 0xFF41
#define LCD_Y_COORDINATE_REG 0xFF44
#define LCD_Y_COMPARE_REG 0xFF45

typedef struct display{
    // x. y, rbg value -> 0 = red, 1 = green, 2 = blue
    uint8_t screen[160][144][3];

    // it takes 456 cycles to draw a scanline
    int scanline_counter;
}display;

void display_init(display *display);
void update_graphics(cpu *cpu_ctx, int cycles);

// LCD control
bool is_LCD_enabled(gb_memory *memory);

/*
    Setting LCD status is probably one of the most confusing part. The LCD goes through 4 different modes. 
    These are "V-Blank Period", "H-Blank Period", "Searching Sprite Attributes" and "Transferring Data to LCD Driver". 
    Bit 1 and 0 of the lcd status at address 0xFF41 reflects the current LCD mode like so:
    00: H-Blank
    01: V-Blank
    10: Searching Sprites Atts
    11: Transfering Data to LCD Driver
    When starting a new scanline the lcd status is set to 0b10, it then moves on to 0b11 and then to 0b00. It then goes back to 
    and continues then pattern until the v-blank period starts where it stays on mode 0b01. 
    When the vblank period ends it goes back to 2 and continues this pattern over and over.
    As previously mentioned it takes 456 clock cycles to draw one scanline before moving onto the next. 
    This can be split down into different sections which will represent the different modes. 
    Mode 2 (Searching Sprites Atts) will take the first 80 of the 456 clock cycles. 
    Mode 3 (Transfering to LCD Driver) will take 172 clock cycles of the 456 and the remaining clock cycles of the 456 
    is for Mode 0 (H-Blank). 
    When the LCD status changes its mode to either Mode 0, 1 or 2 then this can cause an LCD Interupt Request to happen. 
    Bits 3, 4 and 5 of the LCD Status register (0xFF41) are interupt enabled flags (the same as the Interupt Enabled Register 0xFFFF, 
    see interupts). These bits are set by the game not the emulator and they represent the following (bits zero indexed):
    Bit 3: Mode 0 Interupt Enabled
    Bit 4: Mode 1 Interupt Enabled
    Bit 5: Mode 2 Interupt Enabled

    So when the mode changes to 0,1 or 2 then if the corresponding bit 3,4,5 is set then an LCD interupt is requested. 
    This is only tested when the LCD mode changes to 0,1 or 2 and not the duration of these modes.

    **** IMPORTANT Note
        when the lcd is disabled the mode must be set to mode 1.Otherwise Mario2 wont play past the title screen. 
        You also need to reset the m_ScanlineCounter and current scanline
    
    The last part of the LCD status register (0xFF41) is the Coincidence flag. Basically Bit 2 of the status register 
    is set to 1 if register (0xFF44) is the same value as (0xFF45) otherwise it is set to 0. Bit 6 of the LCD status register (0xFF41)
    tells whether coincidence interrupt is enabled or not.
    If the conicidence flag (bit 2) is set and the conincidence interupt enabled flag (bit 6) is set then
    an LCD Interupt is requested. The conicidence flag means the current scanline (0xFF44) is the same as a scanline the game 
    is interested in (0xFF45). The reason why the game would be interested in the current scanline is to do special effects. 
    So when 0xFF44 == 0xFF45 then an interupt can be requested to let the game know that the values are the same.


    LCD mode description:

    Mode 0 - Horizontal Blank (H-Blank):
    In this mode, the LCD is not actively drawing to the screen.
    H-Blank occurs during the period when the horizontal scan is moving from right to left.
    CPU can access VRAM (Video RAM) during this mode.

    Mode 1 - Vertical Blank (V-Blank): set when current scanline is between 144 and 153
    V-Blank occurs during the period when the vertical scan is moving from bottom to top.
    In this mode, the LCD is not actively drawing to the screen, and the entire screen is blank.
    CPU can access VRAM during this mode.

    Mode 2 - OAM (Object Attribute Memory) Search:
    In this mode, the LCD is scanning the OAM, which contains information about sprites (objects) to be drawn on the screen.
    CPU cannot access VRAM during this mode.

    Mode 3 - LCD Drawing:
    In this mode, the LCD is actively drawing graphics from VRAM to the screen.
    The Game Boy has 144 visible scanlines, and each scanline is drawn sequentially from top to bottom.
    During this mode, the CPU cannot access VRAM.
*/ 
void set_LCD_status(cpu *cpu_ctx);