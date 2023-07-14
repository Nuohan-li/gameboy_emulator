
#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "memory.h"
// #include "display.h"
// #include "input.h"

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
} cpu;

void cpu_init(cpu *cpu_ctx, gb_memory *memory);
uint64_t load_game(cpu *cpu_ctx, const char *game_file);
void execute_opcode(cpu *cpu_ctx, uint16_t opcode);

// timer function
void update_timer(cpu *cpu_ctx, int cycles);
bool is_timer_enabled(cpu *cpu_ctx);
uint8_t get_timer_frequency(cpu *cpu_ctx);
void set_timer_counter(cpu *cpu_ctx);

void update_divider_register(cpu *cpu_ctx, int cycles);

// stack
void push(cpu *cpu_ctx, uint8_t value);
void push_two_bytes(cpu *cpu_ctx, uint16_t value);
uint8_t pop(cpu *cpu_ctx);
uint16_t pop_two_bytes(cpu *cpu_ctx);
