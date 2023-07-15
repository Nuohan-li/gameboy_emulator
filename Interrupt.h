// There are two important registers in the memory
//      Interrupt request register 0xFF0F
//      Interrupt enable register 0xFFFF
// When a game wants to request an interrupt, it must set the corresponding bit in 0xFF0F to request a specific interrupt
// 

// Criteria needed to service an interrupt
// 1. make an interrupt request by setting the coresponding bit in 0xFF0F
// 2. make sure the interrupt master enable switch is set to true -> an interrupt is only serviced if this switch is set to true
// 3. if the above two conditions are true and no other higher priority interrupts waiting to be serviced, 
//    it checkes 0xFFFF interrupt_enable to see if the corresponding bit is set to 1 to allow servicing of this particular interrupt


// V-BLANK bit 0: 
// The gameboy draws the display a scanline at a time. There are 144 scanlines on the display 
// and when it has drawn the last one it starts again from the beginning. The time it takes to 
// stop drawing scanline 144 and starting again at scanline 0 is the Vertical Blank period and this 
// is when it requests the v-blank interupt. This is the most important interupt to emulate correctly 
// because during V-Blank the game can read from memory that was previously restricted, mainly video 
// memory. The gameboy has a vertical refresh rate of 60 frames per second meaning 
// that if Step 2 and Step 3 of the above steps is always true then there should be 60 V-Blank interupts 
// a second. we will want to monitor this to make sure you are accurately emulating V-Blank.

// LCD bit 1: 
// There are many reasons why the LCD would request an interupt. The main thing about the LCD 
// interupt is it lets the game know what state the LCD is in because depending what state its 
// in certain memory regions become restricted. The game can also set up a conincidence variable 
// which means "let me know when you're active scanline is X". One of the things that stumped me 
// with the LCD interupt is that it can request an LCD interupt whenever V-Blank happens. I didnt 
// understand why there were two v-blank interupts, the main one which was discussed above and the 
// one nested into the LCD interupt. What you need to remember is that during V-Blank if the game is 
// allowing it there will be two V-Blank interupts requested. The first is the main V-Blank interupt 
// and the second is the LCD interupt. However the first vblank interupt will have the higher priority.

// TIMER bit 2: 
// Basically the gameboy timer counts up at a dynamic frequency and when it gets to value 255 and is 
// about to overflow it lets the game know by requesting the timer interupt.

// Serial transfer completion bit 3:
// This interrupt is only called when gameboy link port transfer is completed 
//      *** This interrupt won't be needed for emulator (probably?)

// JOYPAD bit 4: 
// This interupt is requested whenever one of the buttons goes from the unpressed state to the pressed state. 
#ifndef INTERRUPT_H
#define INTERRUPT_H


#include <stdio.h>
#include "cpu.h"

#define VBLANK 0
#define VBLANK_ROUTINE 0x0040
#define LCD 1
#define LCD_ROUTINE 0x0048
#define TIMER 2
#define TIMER_ROUTINE 0x0050
#define JOYPAD 4
#define JOYPAD_ROUTINE 0x0060

#define INTERRUPT_REQ_REG 0xFF0F
#define INTERRUPT_ENABLE 0XFFFF

void request_interrupt(cpu *cpu_ctx, int bit_position);
void check_interrupt(cpu *cpu_ctx);

// the interrupt routines are stored at the following address:
// V-Blank: 0x40
// LCD: 0x48
// TIMER: 0x50
// JOYPAD: 0x60
// so to invokde these, we simply set program counter to the above address
void service_interrupt(cpu *cpu_ctx, int interrupt);

void interrupt_vblank();
void interrupt_lcd();
void interrupt_timer();
void interrupt_joypad();

#endif