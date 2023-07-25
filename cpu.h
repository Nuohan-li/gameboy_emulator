
#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "memory.h"
#include "display.h"
#include "input.h"

// cpu defines 
// for flag see GBManual P.62
#define CPU_CLOCK_SPEED_HZ 4194304
#define FLAG_Z 7   // zero flag
#define FLAG_N 6   // subtract flag
#define FLAG_H 5   // half carry flag
#define FLAG_C 4   // carry flag

// Timer controller controls the timer (lol)
// bit 1 and 2 (LSB) of the timer controller tells us the frequency at which the timer
// should increments
// 00: 4096 Hz   ----> default, in cpu_inint
// 01: 262144 Hz
// 10: 65536 Hz
// 11: 16384 Hz
// bit 3 tells us whether the timer is enabled or not
#define TIMER_ADDR 0xFF05
#define TIMER_RESET_VALUE_ADDR 0xFF06
#define TIMER_CONTROLLER_ADDR 0xFF07 
#define FREQUENCY_4096_HZ 4096
#define FREQUENCY_262144_HZ 262144
#define FREQUENCY_65536_HZ 65536
#define FREQUENCY_16384_HZ 16384

/*
    The gameboy has 8 registers named A,B,C,D,E,F,H,L each are 8-Bits in size. 
    However these registers are often paired to form 4 16-Bit registers. 
    The pairings are AF, BC, DE, HL. AF is less frequently used then the others 
    because A is the accumulator and F is the flag register so working with them as a 
    pair is less frequent than the others. HL is used very frequently mainly for referring to game memory.

*/
typedef union Registers{
    uint16_t value;
    struct{
        uint8_t lo;  // lo defined first because my windows laptop is little endian
        uint8_t hi;
    }__attribute__((__packed__));
} Registers;

typedef struct cpu{
    Registers AF;
    Registers BC;
    Registers DE;
    Registers HL;
    Registers PC;           
    Registers SP; // some instructions uses only the lower byte or upper byte of the stack pointer     
    
    // memory
    gb_memory *memory;

    // Whenever the timer overflows (memory elements are all unsigned bytes so overflowing means 
    // going greater than 255) it requests a timer interupt and then resets itself to the value of the timer 
    // modulator in memory address 0xFF06.
    // timer_counter will be set to determine whether timer is about to overflow or not
    int timer_counter;
    bool is_timer_enabled;

    // The Divider Register is an 8-bit register located at memory address 0xFF04 in the Game Boy's memory map. 
    // It is automatically incremented at a fixed rate, regardless of the CPU speed or any other operations taking 
    // place in the system.
    // equency at which the Divider Register is incremented is 16384 times per second (or approximately 16.78 kHz). 
    // This means that the value of the register increases by 1 every 59.73 microseconds or every 256 CPU cycles.
    // The Game Boy's CPU generates a clock signal that runs at a specific frequency. 
    // This clock signal is divided down to create other timing signals used by different components of the system, 
    // including the display and the sound. The Divider Register keeps track of the current value of the divided clock signal.

    // game boy manual does not specify the initial value of this register
    // defaulting it to 0 upon cpu initialization (done by memory init), not sure if it's correct?
    int divider_counter;  // this counter counts up to 256, once reached its reset and divider_register value ++

    // interrupt
    bool interrupt_master_switch; // see interrupt.h for explanation

    // display
    struct display *display;

    // input 
    

} cpu;

void cpu_init(cpu *cpu_ctx, gb_memory *memory, display *screen);
uint64_t load_game(cpu *cpu_ctx, const char *game_file);
void execute_opcode(cpu *cpu_ctx, uint16_t opcode);

// timer function
void update_timer(cpu *cpu_ctx, int cycles);
bool is_timer_enabled(cpu *cpu_ctx);
uint8_t get_timer_frequency(cpu *cpu_ctx);
void set_timer_frequency(cpu *cpu_ctx, uint8_t frequency);
void set_timer_counter(cpu *cpu_ctx);

void update_divider_register(cpu *cpu_ctx, int cycles);

// stack
void push(cpu *cpu_ctx, uint8_t value);
void push_two_bytes(cpu *cpu_ctx, uint16_t value);
uint8_t pop(cpu *cpu_ctx);
uint16_t pop_two_bytes(cpu *cpu_ctx);

/////////////////////////////////////// LCD /////////////////////////////////////////

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
bool is_LCD_enabled(gb_memory *memory);

/////////////////// LCD controller //////////////////////
/*
    - Bit 7: checks if the LCD is enabled, if not, we don't draw anything -> handled in update_graphics function 
    - Bit 6: Read the tile identity number to draw onto the screen
    - Bit 5: If this is set to 0 then the window is not enabled so we dont draw it
    - Bit 4:
    - Bit 3: This is the same as Bit6 but for the background not the window
    - Bit 2: This is the size of the sprites that need to draw. Unlike tiles that are always 8x8 sprites can be 8x16
    - Bit 1: Same as Bit5 but for sprites 
    - Bit 0: Same as Bit5 but for background
*/

///////////////////////////////// GRAPHICS //////////////////////////////////////

// The gameboy uses the tile and sprite method of storing and drawing graphics to the screen. The tiles are what form the
// background and are not interactive. Each tile is 8x8 pixels. The sprites are the interactive graphics on the display. 
// An example is the game Mario. The character mario is a sprite. The graphic can move and it can collide with the other 
// sprites. The bad guys are also sprites as they can fly around and attack mario. The tiles are the background which defines
// the level and its terrain.

// As stated previously the resolution of the gameboy is 160x144 however this is just what can be displayed on the screen. 
// The real resolution is 256x256 (32x32 tiles). The visual display can show any 160x144 pixels of the 256x256 background, 
// this allows for scrolling the viewing area over the background.

// Aswell as having a 256x256 background and a 160x144 viewing the display the gameboy has a window which appears above 
// the background but behind the sprites (unless the attributes of the sprite specify otherwise, discussed later). The 
// purpose of the window is to put a fixed panel over the background that does not scroll. For example some games have a 
// panel on the screen which displays the characters health and collected items, (notably links awakening) and this panel 
// does not scroll with the background when the character moves. This is the window.

void update_graphics(cpu *cpu_ctx, int cycles);
void draw_scanline();

void draw_sprite();

/*
    since we are only drawing 160 x 144 pixels out of 256 x 256 pixels, we need to know where from these 256 x 256 pixels
    to start drawing -> scrollx, scrolly, windowx, windowy.
    
    The gameboy has two regions of memoory for the background layout which is shared by the window. The memory regions are 
    0x9800-0x9BFF and 0x9C00-9FFF. We need to check bit 3 of the lcd contol register to see which region we are using for 
    the background and bit 6 for the window. Each byte in the memory region is a tile identification number of what needs 
    to be drawn. This identification number is used to lookup the tile data in video ram so we know how to draw it.

*/
void draw_tiles();


#endif 