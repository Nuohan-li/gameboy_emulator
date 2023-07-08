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

typedef struct gb_memory{
    uint8_t ram[0x10000];
    uint8_t cartridge_memory[ROM_MAX_SIZE_BYTES];
}gb_memory;

void init_gb_memory(gb_memory *memory);
void init_ram(gb_memory *memory);
void init_cartridge(gb_memory *memory);
void set_memory(gb_memory *memory, int address, uint8_t value);
uint8_t memory_get_one_byte(gb_memory *memory, int address);
uint16_t memory_get_two_bytes(gb_memory *memory, int address);
void load_cart_game(gb_memory *memory, uint8_t *game, uint64_t size); 
