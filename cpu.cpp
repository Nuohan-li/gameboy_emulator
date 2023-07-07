#include <iostream>
#include <string.h>
#include <stdint.h>
#include "cpu.h"

using namespace std;

// initialize the CPU -> set everything to 0
void cpu_init(cpu *cpu_ctx){
    memset(cpu_ctx, 0, sizeof(cpu));
}

// memory space starts at 512 or 0x200 in the memory array 
void load_game(cpu *cpu_ctx, uint8_t *game, size_t gamesize){
    memcpy(&cpu_ctx->memory.ram[512], game, gamesize);
    cpu_ctx->program_counter = 512;
}

void execute_opcode(cpu *cpu_ctx, uint16_t opcode){
    // printf("Executing opcode: %04X\n", opcode);
    cout << "Executing opcode: " << opcode << endl;
}