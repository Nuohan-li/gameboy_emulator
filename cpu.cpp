#include <iostream>
#include <string.h>
#include <stdint.h>
#include "cpu.h"

using namespace std;

// initialize the CPU -> set everything to 0, then set the specific 
void cpu::cpu_init(){
    memset(this, 0, sizeof(cpu));

    // when the emulator starts the following values must be set not sure why??
    this->PC.value = 0x100 ;
    this->AF.value = 0x01B0;
    this->BC.value = 0x0013;
    this->DE.value = 0x00D8;
    this->HL.value =0x014D;
    this->SP.value = 0xFFFE;
    this->memory.set_memory(0xFF05, 0x00);
    this->memory.set_memory(0xFF06, 0x00);
    this->memory.set_memory(0xFF07, 0x00);
    this->memory.set_memory(0xFF10, 0x80);
    this->memory.set_memory(0xFF11, 0xBF);
    this->memory.set_memory(0xFF12, 0xF3);
    this->memory.set_memory(0xFF14, 0xBF);
    this->memory.set_memory(0xFF16, 0x3F);
    this->memory.set_memory(0xFF17, 0x00);
    this->memory.set_memory(0xFF19, 0xBF);
    this->memory.set_memory(0xFF1A, 0x7F);
    this->memory.set_memory(0xFF1B, 0xFF);
    this->memory.set_memory(0xFF1C, 0x9F);
    this->memory.set_memory(0xFF1E, 0xBF);
    this->memory.set_memory(0xFF20, 0xFF);
    this->memory.set_memory(0xFF21, 0x00);
    this->memory.set_memory(0xFF22, 0x00);
    this->memory.set_memory(0xFF23, 0xBF);
    this->memory.set_memory(0xFF24, 0x77);
    this->memory.set_memory(0xFF25, 0xF3);
    this->memory.set_memory(0xFF26, 0xF1);
    this->memory.set_memory(0xFF40, 0x91);
    this->memory.set_memory(0xFF42, 0x00);
    this->memory.set_memory(0xFF43, 0x00);
    this->memory.set_memory(0xFF45, 0x00);
    this->memory.set_memory(0xFF47, 0xFC);
    this->memory.set_memory(0xFF48, 0xFF);
    this->memory.set_memory(0xFF49, 0xFF);
    this->memory.set_memory(0xFF4A, 0x00);
    this->memory.set_memory(0xFF4B, 0x00);
    this->memory.set_memory(0xFFFF, 0x00);


}

// memory space starts at 512 or 0x200 in the memory array 
void cpu::load_game(const char *game_file){
    FILE *f = fopen(game_file, "rb");
    fseek(f, 0, SEEK_END);
    uint64_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t game[size];
    fread(game, size, 1, f);
    this->memory.load_game(game, size);
}

void execute_opcode(cpu *cpu_ctx, uint16_t opcode){
    // printf("Executing opcode: %04X\n", opcode);
    cout << "Executing opcode: " << opcode << endl;
}