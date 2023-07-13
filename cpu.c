#include <string.h>
#include <stdint.h>
#include "cpu.h"
#include "memory.h"

// initialize the CPU -> set everything to 0, then set the specific 
void cpu_init(cpu *cpu_ctx, gb_memory *memory){
    cpu_ctx->memory = memory;
    init_gb_memory(memory);

    cpu_ctx->PC.value = 0x100 ;
    cpu_ctx->AF.value = 0x01B0;
    cpu_ctx->BC.value = 0x0013;
    cpu_ctx->DE.value = 0x00D8;
    cpu_ctx->HL.value = 0x014D;
    cpu_ctx->SP.value = 0xFFFE;
    write_byte(cpu_ctx->memory, 0xFF05, 0x00);
    write_byte(cpu_ctx->memory, 0xFF06, 0x00);
    write_byte(cpu_ctx->memory, 0xFF07, 0x00);
    write_byte(cpu_ctx->memory, 0xFF10, 0x80);
    write_byte(cpu_ctx->memory, 0xFF11, 0xBF);
    write_byte(cpu_ctx->memory, 0xFF12, 0xF3);
    write_byte(cpu_ctx->memory, 0xFF14, 0xBF);
    write_byte(cpu_ctx->memory, 0xFF16, 0x3F);
    write_byte(cpu_ctx->memory, 0xFF17, 0x00);
    write_byte(cpu_ctx->memory, 0xFF19, 0xBF);
    write_byte(cpu_ctx->memory, 0xFF1A, 0x7F);
    write_byte(cpu_ctx->memory, 0xFF1B, 0xFF);
    write_byte(cpu_ctx->memory, 0xFF1C, 0x9F);
    write_byte(cpu_ctx->memory, 0xFF1E, 0xBF);
    write_byte(cpu_ctx->memory, 0xFF20, 0xFF);
    write_byte(cpu_ctx->memory, 0xFF21, 0x00);
    write_byte(cpu_ctx->memory, 0xFF22, 0x00);
    write_byte(cpu_ctx->memory, 0xFF23, 0xBF);
    write_byte(cpu_ctx->memory, 0xFF24, 0x77);
    write_byte(cpu_ctx->memory, 0xFF25, 0xF3);
    write_byte(cpu_ctx->memory, 0xFF26, 0xF1);
    write_byte(cpu_ctx->memory, 0xFF40, 0x91);
    write_byte(cpu_ctx->memory, 0xFF42, 0x00);
    write_byte(cpu_ctx->memory, 0xFF43, 0x00);
    write_byte(cpu_ctx->memory, 0xFF45, 0x00);
    write_byte(cpu_ctx->memory, 0xFF47, 0xFC);
    write_byte(cpu_ctx->memory, 0xFF48, 0xFF);
    write_byte(cpu_ctx->memory, 0xFF49, 0xFF);
    write_byte(cpu_ctx->memory, 0xFF4A, 0x00);
    write_byte(cpu_ctx->memory, 0xFF4B, 0x00);
    write_byte(cpu_ctx->memory, 0xFFFF, 0x00);

    // timer counter should be 1024
    set_timer_counter(cpu_ctx);
    cpu_ctx->divider_counter = 256;
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


// TIMER

// testing bit 3 to see if timer is enabled
bool is_timer_enabled(cpu *cpu_ctx){
    uint8_t data = read_one_byte(cpu_ctx->memory, TIMER_CONTROLLER_ADDR);
    if(data & 0b00000100){
        return true;
    } else{
        return false;
    }
}

uint8_t get_timer_frequency(cpu *cpu_ctx){
    return (read_one_byte(cpu_ctx->memory, TIMER_CONTROLLER_ADDR) &0b00000011);
}

void set_timer_counter(cpu *cpu_ctx){
    // cpu_ctx->timer_counter = CPU_CLOCK_SPEED_HZ / get_timer_frequency(cpu_ctx);
    uint8_t bit_pattern = get_timer_frequency(cpu_ctx);
    switch (bit_pattern){
    case 0b00:
        cpu_ctx->timer_counter = 1024; // CPU_CLOCK_SPEED_HZ / 4096 -> timer should increment every 1024 cpu cycles
        break;
    case 0b01:
        cpu_ctx->timer_counter = 16;
        break;
    case 0b10:
        cpu_ctx->timer_counter = 64;
        break;
    case 0b11:
        cpu_ctx->timer_counter = 256;
        break;
    default:
        break;
    }
}

void update_divider_register(cpu *cpu_ctx, int cycles){
    cpu_ctx->divider_counter -= cycles;
    if(cpu_ctx->divider_counter <= 0){
        cpu_ctx->divider_counter = 256;
        cpu_ctx->memory->internal_memory[0xFF04]++;  // incrementing using memory because gameboy doesn't allow games to
                                                   // to do it, if games write to it, it sets this register value to 0
    }
}

// timer (0xFF05) increments at a set frequency, whenever it overflows, it request an interrupt and reset itself to the value storerd at address 0xFF06
void update_timer(cpu *cpu_ctx, int cycles){
    update_divider_register(cpu_ctx, cycles);
    if(is_timer_enabled(cpu_ctx)){
        if(cpu_ctx->timer_counter <= 0){
            set_timer_counter(cpu_ctx);

            if(read_one_byte(cpu_ctx->memory, TIMER_ADDR) == 255){  // timer is about to overflow
                write_byte(cpu_ctx->memory, TIMER_ADDR, read_one_byte(cpu_ctx->memory, TIMER_RESET_VALUE_ADDR));
                request_interrupt(cpu_ctx);  // to be implemented
            } 
        } else {
            int new_timer_value = read_one_byte(cpu_ctx->memory, TIMER_ADDR) + 1;
            write_byte(cpu_ctx->memory, TIMER_ADDR, new_timer_value);
        }
    }
}