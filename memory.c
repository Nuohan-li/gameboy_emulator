#include "memory.h"
#include <string.h>


void init_gb_memory(gb_memory *memory){
    init_cartridge(memory);
    init_ram(memory);
}

void init_ram(gb_memory *memory){
    memset(memory->ram, 0, GB_RAM_SIZE_BYTES);
}

void init_cartridge(gb_memory *memory){
    memset(memory->ram, 0, ROM_MAX_SIZE_BYTES);
}

void set_memory(gb_memory *memory, int address, uint8_t value){
    memory->ram[address] = value;
}

uint8_t memory_get_one_byte(gb_memory *memory, int address){
    return memory->ram[address];
}

uint16_t memory_get_two_bytes(gb_memory *memory, int address){
    uint8_t byte1 = memory_get_one_byte(memory, address);
    uint8_t byte2 = memory_get_one_byte(memory, address + 1);
    return ( (byte1 << 8) | byte2 );
}

void load_cart_game(gb_memory *memory, uint8_t *game, uint64_t size){
    memcpy(memory->cartridge_memory, game, size);
}