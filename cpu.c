#include <string.h>
#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "Interrupt.h"
#include "debug.h"

// initialize the CPU -> set everything to 0, then set the specific 
void cpu_init(cpu *cpu_ctx, gb_memory *memory, display *screen){
    cpu_ctx->memory = memory;
    cpu_ctx->display = screen;
    init_gb_memory(memory);

    cpu_ctx->PC.value = 0x100 ;
    cpu_ctx->AF.value = 0x01B0;
    cpu_ctx->BC.value = 0x0013;
    cpu_ctx->DE.value = 0x00D8;
    cpu_ctx->HL.value = 0x014D;
    cpu_ctx->SP.value = STACK_ADDR;
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
    
    // display init
    display_init(cpu_ctx->display);
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

void set_timer_frequency(cpu *cpu_ctx, uint8_t frequency){
    if(frequency > 4 || frequency < 0){
        log_trace(true, "cpu.c set_timer_frequency: frequency (lower 2 bits of 0xFF07) must be a value between 0 and 4");
        return;
    }
    uint8_t current_timer_ctrl_val = read_one_byte(cpu_ctx->memory, TIMER_CONTROLLER_ADDR) ;
    uint8_t timer_freq = (current_timer_ctrl_val & 0b11111100) | frequency;  
    write_byte(cpu_ctx->memory, TIMER_CONTROLLER_ADDR, timer_freq);
    set_timer_counter(cpu_ctx);
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
                request_interrupt(cpu_ctx, TIMER);  // to be implemented
            } 
        } else {
            int new_timer_value = read_one_byte(cpu_ctx->memory, TIMER_ADDR) + 1;
            write_byte(cpu_ctx->memory, TIMER_ADDR, new_timer_value);
        }
    }
}

// Stack

void push(cpu *cpu_ctx, uint8_t value){
    if(cpu_ctx->SP.value >= 0xFFFE){
        log_trace(true, "cpu.c push: Stack overflowed");
        return;
    }
    write_byte(cpu_ctx->memory, cpu_ctx->SP.value, value);
    cpu_ctx->SP.value++;
}

void push_two_bytes(cpu *cpu_ctx, uint16_t value){
    if(cpu_ctx->SP.value >= 0xFFFD){
        log_trace(true, "cpu.c push_two_bytes: Stack overflowed");
        return;
    }
    memcpy(&cpu_ctx->memory->internal_memory[cpu_ctx->SP.value], &value, sizeof(uint16_t));
    cpu_ctx->SP.value += 2;
}

// SP points to 1 address above the top item in stack
uint8_t pop(cpu *cpu_ctx){
    if(cpu_ctx->SP.value <= STACK_ADDR){
        log_trace(true, "cpu.c pop: Nothing to pop");
        return -1;
    }
    cpu_ctx->SP.value--;
    return read_one_byte(cpu_ctx->memory, cpu_ctx->SP.value);
}

uint16_t pop_two_bytes(cpu *cpu_ctx){
    if(cpu_ctx->SP.value <= STACK_ADDR + 1){
        log_trace(true, "cpu.c pop: Nothing to pop");
        return -1;
    }
    cpu_ctx->SP.value -= 2;
    return read_two_bytes(cpu_ctx->memory, cpu_ctx->SP.value);
}


// LCD control
bool is_LCD_enabled(gb_memory *memory){
    return (read_one_byte(memory, LCD_CTRL_REG_ADDR) & 0b10000000);
}

void set_LCD_status(cpu *cpu_ctx){
    uint8_t LCD_status = read_one_byte(cpu_ctx->memory, LCD_STATUS_REG);

    // if LCD is disabled
    if(!is_LCD_enabled(cpu_ctx->memory)){
        cpu_ctx->display->scanline_counter = SCALINE_CLK_COUNTER_INIT_VAL;
        write_byte(cpu_ctx->memory, LCD_Y_COORDINATE_REG, 0);

        // getting current LCD status from register except for lower 2 bits 
        uint8_t status = LCD_status & 0b11111100;
        // setting it to mode 1 
        status |= 0b01;
        // update LCD status
        write_byte(cpu_ctx->memory, LCD_STATUS_REG, status);
        return; 
    }

    uint8_t current_line = read_one_byte(cpu_ctx->memory, LCD_Y_COORDINATE_REG);
    uint8_t current_mode = LCD_status & 0b11;
    uint8_t mode = 0;
    bool interrupt_request = false;

    // setting current mode to vBlank as scanline # goes over 143
    if(current_line >= 144){
        mode = 1;
        LCD_status = (LCD_status & 0b11111100) | 0b01;
        write_byte(cpu_ctx->memory, LCD_STATUS_REG, LCD_status);

        // LCD status bit 4 (zero indexed) for mode 1 interrupt
        interrupt_request = LCD_status & 0b00010000;
    } else {
        // mode 2 takes the first 80 cycles of 456 cycles, since we are counting down, hence - 80 here
        int mode2_bound = SCALINE_CLK_COUNTER_INIT_VAL - 80;
        // like mode 2, mode 3 takes 172 cycles out of the remaining 456 - 80 cycles
        int mode3_bound = mode2_bound - 172;

        // mode 2
        if(cpu_ctx->display->scanline_counter >= mode2_bound){
            mode = 2;
            LCD_status = (LCD_status & 0b11111100) | 0b10;
            write_byte(cpu_ctx->memory, LCD_STATUS_REG, LCD_status);
            interrupt_request = LCD_status & 0b00100000;
        }
        // mode 3, it doesn't have interrupt
        else if(cpu_ctx->display->scanline_counter >= mode3_bound){
            mode = 3;
            LCD_status = (LCD_status & 0b11111100) | 0b11;
            write_byte(cpu_ctx->memory, LCD_STATUS_REG, LCD_status);
        }
        else {
            mode = 0;
            LCD_status = LCD_status & 0b11111100;
            write_byte(cpu_ctx->memory, LCD_STATUS_REG, LCD_status);
            interrupt_request = LCD_status & 0b00001000;
        }
    }

    // request interrupt as we just entered a new mode
    if(interrupt_request && mode != current_mode){
        request_interrupt(cpu_ctx, LCD);
    }

    // check coincidence flag
    if(read_one_byte(cpu_ctx->memory, LCD_Y_COMPARE_REG) == read_one_byte(cpu_ctx->memory, LCD_Y_COORDINATE_REG)){
        LCD_status = (LCD_status & 0b11111011) | 0b00000100;
        // checking 7th bit to see if interrupt needs to be invoked
        if(LCD_status & 0b01000000){
            request_interrupt(cpu_ctx, LCD);
        }
    } else{
        LCD_status = LCD_status & 0b11111011;
    }
    write_byte(cpu_ctx->memory, LCD_STATUS_REG, LCD_status);
}

void update_graphics(cpu *cpu_ctx, int cycles){
    set_LCD_status(cpu_ctx);

    if(is_LCD_enabled(cpu_ctx->memory)){
        cpu_ctx->display->scanline_counter -= cycles;
    } else{
        return;
    }

    if(cpu_ctx->display->scanline_counter <= 0){

        // move onto next scanline
        cpu_ctx->memory->internal_memory[LCD_Y_COORDINATE_REG]++;
        uint8_t current_line = read_one_byte(cpu_ctx->memory, LCD_Y_COORDINATE_REG);

        cpu_ctx->display->scanline_counter = SCALINE_CLK_COUNTER_INIT_VAL;

        // entering vblank
        if(current_line == 144){
            request_interrupt(cpu_ctx, VBLANK);
        }

        // going past vblank, reset Y coordinate to 0
        else if(current_line > 153){
            cpu_ctx->memory->internal_memory[LCD_Y_COORDINATE_REG] = 0;
        }

        else if(current_line < 144){
            // draw current scanline
        }
    }
}