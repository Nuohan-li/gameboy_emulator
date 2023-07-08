
#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "memory.h"
// #include "display.h"
// #include "input.h"


/*
    The gameboy has 8 registers named A,B,C,D,E,F,H,L each are 8-Bits in size. 
    However these registers are often paired to form 4 16-Bit registers. 
    The pairings are AF, BC, DE, HL. AF is less frequently used then the others 
    because A is the accumulator and F is the flag register so working with them as a 
    pair is less frequent than the others. HL is used very frequently mainly for referring to game memory.

*/
union Registers{
    uint16_t value;
    struct{
        uint8_t lo;  // lo defined first because my windows laptop is little endian
        uint8_t hi;
    }__attribute__((__packed__));
};

typedef struct cpu{
    // struct registers{
    //     uint8_t A;
    //     uint8_t B;
    //     uint8_t C;
    //     uint8_t D;
    //     uint8_t E;
    //     uint8_t F;
    //     uint8_t H;
    //     uint8_t L;
    // }registers;

    Registers AF;
    Registers BC;
    Registers DE;
    Registers HL;
    uint16_t PC;
    // some instructions uses only the lower byte or upper byte of the stack pointer                
    uint16_t SP; 

    gb_memory memory;
} cpu;

void cpu_init(cpu *cpu_ctx);
uint64_t load_game(cpu *cpu_ctx, const char *game_file);
void execute_opcode(cpu *cpu_ctx, uint16_t opcode);
