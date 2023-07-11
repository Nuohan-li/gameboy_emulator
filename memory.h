#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

/*
https://gbdev.io/pandocs/Memory_Map.html
MEMORY MAP
+-----------+
| 0000-3FFF | 16KB ROM Bank 00 (in cartridge, fixed at rom bank 00)
+-----------+
| 4000-7FFF | 16KB ROM Bank 01..NN (in cartridge, switchable rom bank number)
+-----------+
| 8000-9FFF | 8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
+-----------+
| A000-BFFF | 8KB External RAM (in cartridge, switchable RAM bank, if any) -> not used by MCB1, used by MCB2
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

// reserved memory location shown in section 2.5.4 of gameboy manual
typedef enum rom_banking_modes{
    ROM_ONLY = 0x00,
    MBC1 = 0x01,
    MBC1_RAM = 0x02,
    MBC1_RAM_BATTERY = 0x03,
    MBC2 = 0x05,
    MBC2_BATTERY = 0x06,
    ROM_RAM = 0x08,
    ROM_RAM_BATTERY = 0x09,
    MMM01 = 0x0B,
    MMM01_RAM = 0x0C,
    MMM01_RAM_BATTERY = 0x0D,
    MBC3_TIMER_BATTERY = 0x0F,
    MBC3_TIMER_RAM_BATTERY = 0x10,
    MBC3 = 0x11,
    MBC3_RAM = 0x12,
    MBC3_RAM_BATTERY = 0x13,
    MBC5 = 0x19,
    MBC5_RAM = 0x1A,
    MBC5_RAM_BATTERY = 0x1B,
    MBC5_RUMBLE = 0x1C,
    MBC5_RUMBLE_RAM = 0x1D, 
    MBC5_RUMBLE_RAM_BATTERY = 0x1E,
    POCKET_CAMERA = 0x1F,
    BANDAI_TAMA5 = 0xFD,
    HUDSON_HUC_3 = 0xFE,
    HUDSON_HUC_1 = 0xFF
} rom_banking_modes;

typedef struct gb_memory{
    uint8_t cartridge_memory[ROM_MAX_SIZE_BYTES];
    uint8_t internal_memory[GB_RAM_SIZE_BYTES];
    rom_banking_modes rom_banking_mode;
    uint8_t current_rom_bank;  // rom bank 0 to rom bank 3
    uint8_t current_ram_bank;
    bool enable_ram;
    bool use_rom_banking;
    
} gb_memory;



void init_gb_memory(gb_memory *memory);
void init_ram(gb_memory *memory);
void init_cartridge(gb_memory *memory);
int write_byte(gb_memory *memory, int address, uint8_t value);
uint8_t read_one_byte(gb_memory *memory, int address);
uint16_t read_two_bytes(gb_memory *memory, int address);
void load_cart_game(gb_memory *memory, uint8_t *game, uint64_t size); 
void set_rom_banking_mode(gb_memory *memory);
rom_banking_modes get_rom_banking_mode(gb_memory *memory);