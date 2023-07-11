#include "memory.h"
#include "debug.h"
#include <string.h>


void init_gb_memory(gb_memory *memory){
    memset(memory, 0, sizeof(gb_memory));
    memory->rom_banking_mode = ROM_ONLY;
    memory->current_rom_bank = 0;
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


/// @brief
//      In order to write to RAM banks the game must specifically request that ram bank writing is enabled. 
//      It does this by attempting to write to internal ROM address between 0 and 0x2000. For MBC1 if the lower
//      nibble of the data the game is writing to memory is 0xA then ram bank writing is enabled else if the lower 
//      nibble is 0 then ram bank writing is disabled. MBC2 is exactly the same except there is an additional clause 
//      that bit 4 of the address byte must be 0.  
// Only supports MBC1 and MBC2 for now
/// @param address 
/// @param data 
bool enable_ram(gb_memory *memory, uint16_t address, uint8_t data){
    if(memory->rom_banking_mode == MBC2){
        // address & 1000 = 1 then do not enable ram
        if(address & 0x8){
            return;
        }
        uint8_t test_data = data & 0xF;
        if(test_data == 0xA){
            return true;
        }
        else{
            return false;
        }
    }
}

void change_rom_bank_lo(gb_memory *memory, uint8_t data){
    if(memory->rom_banking_mode == MBC2){
        memory->current_rom_bank = data & 0xF;
        if(memory->current_rom_bank == 0){
            memory->current_rom_bank++;
            return;
        }
    }
    uint8_t lower5 = data & 0b11111; // get lower 5 bits
    memory->current_rom_bank &= 0b11100000; // turn off the lower 5 bits
    memory->current_rom_bank |= lower5;
    if(memory->current_rom_bank == 0){
        memory->current_rom_bank++;
    }
}

void change_rom_bank_hi(gb_memory *memory, uint8_t data){
    // turn off the upper 3 bits
    memory->current_rom_bank &= 0b00011111;
    // turn off lower 5 bits of data
    data &= 0b11100000;
    memory->current_rom_bank |= data;
    if(memory->current_rom_bank == 0){
        memory->current_rom_bank++;
    }
}