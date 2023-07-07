// https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908

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

class cpu{
    private:
        Registers AF;
        Registers BC;
        Registers DE;
        Registers HL;
        Registers PC;
        // some instructions uses only the lower byte or upper byte of the stack pointer                
        Registers SP; 
        gb_memory memory;
    public:
        void cpu_init();
        void load_game(const char *game_file);
        void execute_opcode(uint16_t opcode);

};

// // initialize the CPU -> set everything to 0
// void cpu_init(cpu *cpu);

// // load th game into the memory 
// void load_game(cpu *cpu, uint8_t *game, size_t gamesize);

// void execute_opcode(cpu *cpu, uint16_t opcode);