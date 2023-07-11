#include "memory.h"
#include "debug.h"
#include <string.h>


void init_gb_memory(gb_memory *memory){
    memset(memory, 0, sizeof(gb_memory));
    memory->rom_banking_mode = ROM_ONLY;
    memory->current_rom_bank = 0;
    memory->current_ram_bank = 0;
    memory->enable_ram = false;
    memory->use_rom_banking = true;
}

int write_byte(gb_memory *memory, int address, uint8_t data){
    if(address < 0 || address > 0xFFFF){
        log_trace(1, "Memory error, function set_memory: invalide memory address - address %02X", address);
        return 1;
    }
    if(address >= 0x0000 && address < 0x8000){
        banking_handler(memory, address, data);
    } 

    else if(address >= 0x8000 && address < 0xA000){
        memory->internal_memory[address] = data;
    } 

    else if(address >= 0xA000 && address < 0xC000){
        if(memory->enable_ram){
            memory->internal_memory[address] = data;
        }
    }

    else if(address >= 0xC000 && address < 0xE000){
        memory->internal_memory[address] = data;
    }
    
    else if(address >= 0xE000 && address < 0xFE00){
        log_trace(1, "Memory error, function set_memory: attempting to write at address 0xE000 to 0xFDFF (ECHO) - address %02X", address);
        return 1;
    }

    else if(address >= 0xFE00 && address < 0xFEA0){
        memory->internal_memory[address] = data;
    }

    else if(address >= 0xFEA0 && address < 0xFF00){
        log_trace(1, "Memory error, function set_memory: attempting to write at address 0xFEA0 - 0xFEFF (unused) - address %02X", address);
        return 1;
    }

    else if(address >= 0xFF00 && address < 0xFFFF){
        memory->internal_memory[address] = data;
    }

    else if (address == 0xFFFF){
        if(data != 0x00 && data != 0x01){
            log_trace(1, "Memory error, function set_memory: interrupt enable can be either 1 or 0 - data %02X", data);
            return 1;
        }
        memory->internal_memory[0xFFFF]= data;
    }
    
    return 0;
}

uint8_t read_one_byte(gb_memory *memory, int address){
    if(address < 0 || address > 0xFFFF){
        log_trace(1, "Memory error, function read_one_byte: invalide memory address - address %02X", address);
        return -1;
    }
    if(address >= 0x0000 && address < 0xE000){
        return memory->internal_memory[address];
    } 
    
    else if(address >= 0xE000 && address < 0xFE00){
        log_trace(1, "Memory error, function read_one_byte: attempting to read at address 0xE000 to 0xFDFF (ECHO) - address %02X", address);
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

uint16_t read_two_bytes(gb_memory *memory, int address){
    uint8_t byte1 = read_one_byte(memory, address);
    uint8_t byte2 = read_one_byte(memory, address + 1);
    return ( (byte1 << 8) | byte2 );
}

void load_cart_game(gb_memory *memory, uint8_t *game, uint64_t size){
    memcpy(memory->cartridge_memory, game, size);
}

void set_rom_banking_mode(gb_memory *memory){
    memory->rom_banking_mode = memory->cartridge_memory[0x147];
}


////////////////////////////////////////////////////////////////////////////////////////
//
//  ROM and RAM banking handler       
// 
///////////////////////////////////////////////////////////////////////////////////////

//      In order to write to RAM banks the game must specifically request that ram bank writing is enabled. 
//      It does this by attempting to write to internal ROM address between 0 and 0x2000. For MBC1 if the lower
//      nibble of the data the game is writing to memory is 0xA then ram bank writing is enabled else if the lower 
//      nibble is 0 then ram bank writing is disabled. MBC2 is exactly the same except there is an additional clause 
//      that bit 4 of the address byte must be 0.  
// Only supports MBC1 and MBC2 for now
bool enable_ram(gb_memory *memory, uint16_t address, uint8_t data){
    if(memory->rom_banking_mode == MBC2){
        // address & 1000 = 1 then do not enable ram
        if(address & 0x8){
            return false;
        }
        uint8_t test_data = data & 0xF;
        if(test_data == 0xA){
            return true;
        }
        else{
            return false;
        }
    }
    return false;
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

// You cannot change RAM Banks in MBC2 as that has external ram on the cartridge. 
// To change RAM Banks in MBC1 the game must again write to memory address 0x4000-0x6000 
// but this time m_RomBanking must be false. 
void change_ram_bank(gb_memory *memory, uint8_t data){
    if(memory->rom_banking_mode == MBC2){
        log_trace(true, "Memory warning, change_ram_bank: Attempting to change RAM bank when ROM banking mode is set to MBC2");
        return;
    }
    if(memory->use_rom_banking){
        log_trace(true, "Memory error, change_ram_bank: Attempting to change RAM bank when ROM bank changing is enabled");
        return;
    }
    memory->current_ram_bank = data & 0b00000011;

}

// This variable, rom_banking_mode is responsible for how to act when the game writes to memory address 0x4000-0x6000. 
// This variable defaults to true but is changed during MBC1 mode when the game writes to memory 
// address 0x6000-0x8000. If the least significant bit of the data being written to this address 
// range is 0 then rom_banking_mode is set to true, otherwise it is set to false meaning there is about 
// to be a ram bank change. It is important to set current_ram_bank to 0 whenever you set rom_banking_mode 
// to true because the gameboy can only use rambank 0 in this mode. 
void change_bank_mode(gb_memory *memory, uint8_t data){
    data &= 0b1;
    if(data == 0){
        memory->use_rom_banking = true;
        memory->current_rom_bank = 0;
    } else{
        memory->use_rom_banking = false;
    }
}

void banking_handler(gb_memory *memory, uint16_t address, uint8_t data){
   // do RAM enabling
    if (address < 0x2000){
        if (memory->rom_banking_mode == MBC1 || memory->rom_banking_mode == MBC2){
        enable_ram(memory, address, data);
        }
    }

    // do ROM bank change
    else if ((address >= 0x200) && (address < 0x4000)){
        if (memory->rom_banking_mode == MBC1 || memory->rom_banking_mode == MBC2){
        change_rom_bank_lo(memory, data) ;
        }
    }

    // do ROM or RAM bank change
    else if ((address >= 0x4000) && (address < 0x6000)){
        // there is no rambank in mbc2 so always use rambank 0
        if (memory->rom_banking_mode == MBC1){
        if(memory->use_rom_banking){
            change_rom_bank_hi(memory, data) ;
        }
        else{
            change_ram_bank(memory, data) ;
        }
        }
    }

    // this will change whether we are doing ROM banking
    // or RAM banking with the above if statement
    else if ((address >= 0x6000) && (address < 0x8000)){
        if (memory->rom_banking_mode == MBC1){
            change_bank_mode(memory, data) ;
        }
    }
}