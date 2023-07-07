#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"

/*
MEMORY MAP
+-----------+
| 0000-3FFF | 16KB ROM Bank 00 (in cartridge, fixed at bank 00)
+-----------+
| 4000-7FFF | 16KB ROM Bank 01..NN (in cartridge, switchable bank number)
+-----------+
| 8000-9FFF | 8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
+-----------+
| A000-BFFF | 8KB External RAM (in cartridge, switchable bank, if any)
+-----------+
| C000-CFFF | 4KB Work RAM Bank 0 (WRAM)
+-----------+
| D000-DFFF | 4KB Work RAM Bank 1 (WRAM) (switchable bank 1-7 in CGB Mode)
+-----------+
| E000-FDFF | Same as C000-DDFF (ECHO) (typically not used)
+-----------+
| FE00-FE9F | Sprite Attribute Table (OAM)
+-----------+
| FEA0-FEFF | Not Usable
+-----------+
| FF00-FF7F | I/O Ports
+-----------+
| FF80-FFFE | High RAM (HRAM)
+-----------+
| FFFF      | Interrupt Enable Register
+-----------+
*/ 

class gb_memory{
    private:
        uint8_t ram[GB_RAM_SIZE_BYTES];
        uint8_t cartridge_memory[ROM_MAX_SIZE_BYTES];
    public:
        void init_gb_memory();
        void init_ram();
        void init_cartridge();
        void set_memory(int address, uint8_t value);
        uint8_t memory_get_one_byte(int address);
        uint16_t memory_get_two_bytes(int address);
        void load_game(uint8_t *game, uint64_t size);      
};

