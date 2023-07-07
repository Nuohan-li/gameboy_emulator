#include <iostream>
#include <string.h>
#include "memory.h"


void gb_memory::init_ram(){
    memset(this->ram, 0, GB_RAM_SIZE_BYTES);
}

void gb_memory::init_cartridge(){
    memset(this->ram, 0, ROM_MAX_SIZE_BYTES);
}

void gb_memory::init_gb_memory(){
    init_ram();
    init_cartridge();
}

void gb_memory::set_memory(int address, uint8_t value){
    this->ram[address] = value;
}
uint8_t gb_memory::memory_get_one_byte(int address){
    return this->ram[address];
}
uint16_t gb_memory::memory_get_two_bytes(int address){
    uint8_t byte1 = memory_get_one_byte(address);
    uint8_t byte2 = memory_get_one_byte(address + 1);
    return ((byte1 << 8) | byte2);
}

void gb_memory::load_game(uint8_t *game, uint64_t size){
    memcpy(this->cartridge_memory, game, size);
}