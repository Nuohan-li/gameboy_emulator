#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"

/*
https://gbdev.io/pandocs/Memory_Map.html
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

struct internal_memory{
    // 0x0000 to 0x3FFF
    uint8_t ROM_bank0[0x4000]; 
    // 0x4000 to 0x7FFF
    uint8_t ROM_bank1[0x4000]; 
    // 0x8000 to 0x9FFF
    uint8_t vram[0x2000]; 
    // 0xA000 t0 0xBFFF
    uint8_t external_ram[0x2000]; // ram banking
    // 0xC000 to 0xCFFF
    uint8_t wram0[0x1000];
    // 0xD000 to 0xDFFF
    uint8_t wram1[0x1000];
    // 0xE000 to 0xFDFF
    // uint8_t ECHO_ram[0x1DFF]; // typically not used
    // 0xFE00 to 0xFE9F 
    uint8_t oam[0x100];
    // 0xFEA0 - 0xFEFF Not Usable
    // 0xFF00 to 0xFF7F I/O Ports
    uint8_t io[0x7F];
    // FF80-FFFE High RAM (HRAM)
    uint8_t hram[0x7E];
    uint8_t interrupt_enable;
} __attribute__((packed));

typedef struct gb_memory{
    uint8_t cartridge_memory[ROM_MAX_SIZE_BYTES];
    struct internal_memory internal_memory;
    
} gb_memory;

void init_gb_memory(gb_memory *memory);
void init_ram(gb_memory *memory);
void init_cartridge(gb_memory *memory);
int set_memory(gb_memory *memory, int address, uint8_t value);
uint8_t memory_get_one_byte(gb_memory *memory, int address);
uint16_t memory_get_two_bytes(gb_memory *memory, int address);
void load_cart_game(gb_memory *memory, uint8_t *game, uint64_t size); 
