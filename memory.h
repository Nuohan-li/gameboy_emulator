#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"

class gb_memory{
    private:
        uint8_t ram[GB_RAM_SIZE];
        uint8_t cartridge_memory[ROM_MAX_SIZE_BYTES];
    public:
        void init_gb_memory();
        void set_memory(int address, uint8_t value);
        uint8_t memory_get_one_byte(int address);
        uint16_t memory_get_two_bytes(int address);
        
};

// this struct emulates the memory module
typedef struct memory{
    uint8_t ram[GB_RAM_SIZE];
    uint16_t stack[16];
}memory; 

// this function initializes the memory module by setting the content of the memory to 0
void memory_init(memory *mem);

// this function sets one byte of memory at "address" to "value"
void memory_set(memory *mem, int address, uint8_t value);

// this function returns one byte of memory content at "address"
uint8_t memory_get_one_byte(memory *mem, int address);

// this function returns two bytes of memory content beginning at "address"
uint16_t memory_get_two_bytes(memory *mem, int address);

void push(memory *mem, uint16_t value);

uint16_t pop(memory *mem);