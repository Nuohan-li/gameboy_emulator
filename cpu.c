#include <string.h>
#include <stdint.h>
#include "cpu.h"
#include "memory.h"

// initialize the CPU -> set everything to 0, then set the specific 
void cpu_init(cpu *cpu_ctx, gb_memory *memory){
    cpu_ctx->memory = memory;

    cpu_ctx->PC.value = 0x100 ;
    cpu_ctx->AF.value = 0x01B0;
    cpu_ctx->BC.value = 0x0013;
    cpu_ctx->DE.value = 0x00D8;
    cpu_ctx->HL.value = 0x014D;
    cpu_ctx->SP.value = 0xFFFE;
    set_memory(cpu_ctx->memory, 0xFF05, 0x00);
    set_memory(cpu_ctx->memory, 0xFF06, 0x00);
    set_memory(cpu_ctx->memory, 0xFF07, 0x00);
    set_memory(cpu_ctx->memory, 0xFF10, 0x80);
    set_memory(cpu_ctx->memory, 0xFF11, 0xBF);
    set_memory(cpu_ctx->memory, 0xFF12, 0xF3);
    set_memory(cpu_ctx->memory, 0xFF14, 0xBF);
    set_memory(cpu_ctx->memory, 0xFF16, 0x3F);
    set_memory(cpu_ctx->memory, 0xFF17, 0x00);
    set_memory(cpu_ctx->memory, 0xFF19, 0xBF);
    set_memory(cpu_ctx->memory, 0xFF1A, 0x7F);
    set_memory(cpu_ctx->memory, 0xFF1B, 0xFF);
    set_memory(cpu_ctx->memory, 0xFF1C, 0x9F);
    set_memory(cpu_ctx->memory, 0xFF1E, 0xBF);
    set_memory(cpu_ctx->memory, 0xFF20, 0xFF);
    set_memory(cpu_ctx->memory, 0xFF21, 0x00);
    set_memory(cpu_ctx->memory, 0xFF22, 0x00);
    set_memory(cpu_ctx->memory, 0xFF23, 0xBF);
    set_memory(cpu_ctx->memory, 0xFF24, 0x77);
    set_memory(cpu_ctx->memory, 0xFF25, 0xF3);
    set_memory(cpu_ctx->memory, 0xFF26, 0xF1);
    set_memory(cpu_ctx->memory, 0xFF40, 0x91);
    set_memory(cpu_ctx->memory, 0xFF42, 0x00);
    set_memory(cpu_ctx->memory, 0xFF43, 0x00);
    set_memory(cpu_ctx->memory, 0xFF45, 0x00);
    set_memory(cpu_ctx->memory, 0xFF47, 0xFC);
    set_memory(cpu_ctx->memory, 0xFF48, 0xFF);
    set_memory(cpu_ctx->memory, 0xFF49, 0xFF);
    set_memory(cpu_ctx->memory, 0xFF4A, 0x00);
    set_memory(cpu_ctx->memory, 0xFF4B, 0x00);
    set_memory(cpu_ctx->memory, 0xFFFF, 0x00);
}

uint64_t load_game(cpu *cpu_ctx, const char *game_file){
    FILE *f = fopen(game_file, "rb");
    fseek(f, 0, SEEK_END);
    uint64_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t game[size];
    fread(cpu_ctx->memory->cartridge_memory, size, 1, f);
    // load_cart_game(cpu_ctx->memory, game, size);
    return size;
}


void execute_opcode(cpu *cpu_ctx, uint16_t opcode){
    printf("Executing opcode: %04X\n", opcode);
}