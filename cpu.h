// https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908

#pragma once

#include <stdio.h>
#include <stdint.h>
#include "memory.h"
// #include "display.h"
// #include "input.h"

// this struct emulates the registers -> section 2.2 in the reference
typedef struct cpu{
    // V array represents 16 one byte general purpose registers used by CHIP8
    uint8_t V[16];
    // special 16 bit register in chip-8, used to store memory address
    uint16_t I;
    // see section 2.5 reference
    uint8_t delay_timer;
    // see section 2.5 reference
    uint8_t sound_timer;
    // 16bit, points to the next instruction that will be run after the current one
    uint16_t program_counter;
    // 8 bit, points to the topmost level of the stack
    uint8_t stack_pointer;

    // different components defined in other header files
    // input input;
    memory memory;
    // display display;

} cpu;

// initialize the CPU -> set everything to 0
void cpu_init(cpu *cpu);

// load th game into the memory 
void load_game(cpu *cpu, uint8_t *game, size_t gamesize);

void execute_opcode(cpu *cpu, uint16_t opcode);