#include <stdio.h>
#include <string.h>
#include "display.h"
#include "Interrupt.h"

void display_init(display *display){
    memset(display->screen, 0, sizeof(display->screen));
    display->scanline_counter = SCALINE_CLK_COUNTER_INIT_VAL;
}

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