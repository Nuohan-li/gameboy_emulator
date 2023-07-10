#include "memory.h"
#include "debug.h"
#include <string.h>


void init_gb_memory(gb_memory *memory){
    memset(memory, 0, sizeof(gb_memory));
}

// void init_ram(gb_memory *memory){
//     memset(memory->ram, 0, GB_RAM_SIZE_BYTES);
// }

// void init_cartridge(gb_memory *memory){
//     memset(memory->ram, 0, ROM_MAX_SIZE_BYTES);
// }

int set_memory(gb_memory *memory, int address, uint8_t value){
    if(address < 0 || address > 0xFFFF){
        log_trace(1, "Memory error, function set_memory: invalide memory address - address %02X", address);
        return 1;
    }
    if(address >= 0x0000 && address < 0x8000){
        log_trace(1, "Memory error, function set_memory: attempting to write at address between 0x0000 to 0x4000 (read-only) - address %02X", address);
        // printf("size %I64u\n", sizeof(gb_memory));
        return 1;
    } 
    // else if(address >= 0x4000 && address < 0x8000){
    //     memory->internal_memory.ROM_bank1[address - 0x4000] = value;
    // } 
    else if(address >= 0x8000 && address < 0xE000){
        memory->internal_memory[address] = value;
    } 
    
    else if(address >= 0xE000 && address < 0xFE00){
        log_trace(1, "Memory error, function set_memory: attempting to write at address 0xE000 to 0xFDFF (ECHO) - address %02X", address);
        return 1;
    }
    else if(address >= 0xFE00 && address < 0xFEA0){
        memory->internal_memory[address] = value;
    }
    else if(address >= 0xFEA0 && address < 0xFF00){
        log_trace(1, "Memory error, function set_memory: attempting to write at address 0xFEA0 - 0xFEFF (unused) - address %02X", address);
        return 1;
    }
    else if(address >= 0xFF00 && address < 0xFFFF){
        memory->internal_memory[address] = value;
    }
    else if (address == 0xFFFF){
        if(value != 0x00 && value != 0x01){
            log_trace(1, "Memory error, function set_memory: interrupt enable can be either 1 or 0 - value %02X", value);
            return 1;
        }
        memory->internal_memory[0xFFFF]= value;
    }
    
    return 0;
}

uint8_t memory_get_one_byte(gb_memory *memory, int address){
    if(address < 0 || address > 0xFFFF){
        log_trace(1, "Memory error, function memory_get_one_byte: invalide memory address - address %02X", address);
        return -1;
    }
    if(address >= 0x0000 && address < 0xE000){
        return memory->internal_memory[address];
    } 
    
    else if(address >= 0xE000 && address < 0xFE00){
        log_trace(1, "Memory error, function memory_get_one_byte: attempting to read at address 0xE000 to 0xFDFF (ECHO) - address %02X", address);
        return -1;
    }
    else if(address >= 0xFE00 && address < 0xFEA0){
        return memory->internal_memory[address];
    }
    else if(address >= 0xFEA0 && address < 0xFF00){
        log_trace(1, "Memory error, function set_memory: attempting to read at address 0xFEA0 - 0xFEFF (unused) - address %02X", address);
        return -1;
    }
    else if(address >= 0xFF00 && address <= 0xFFFF){
        return memory->internal_memory[address];
    }
    return -1;    
}

uint16_t memory_get_two_bytes(gb_memory *memory, int address){
    uint8_t byte1 = memory_get_one_byte(memory, address);
    uint8_t byte2 = memory_get_one_byte(memory, address + 1);
    return ( (byte1 << 8) | byte2 );
}

void load_cart_game(gb_memory *memory, uint8_t *game, uint64_t size){
    memcpy(memory->cartridge_memory, game, size);
}

void set_rom_banking_mode(gb_memory *memory){
    memory->rom_banking_mode = memory->cartridge_memory[0x147];
    
}