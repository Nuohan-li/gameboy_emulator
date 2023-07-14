#include "Interrupt.h"

void request_interrupt(cpu  *cpu_ctx, int bit_position){
    // uint16_t request = read_one_byte(cpu_ctx->memory, INTERRUPT_REQ_REG);
    switch (bit_position){
    case VBLANK_BIT_POS:
        write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00000001);
        break;
    case LCD_BIT_POS:
        write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00000010);
        break;
    case TIMER_BIT_POS:
        write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00000100);
        break;
    case JOYPAD_BIT_POS:
        write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00010000);
    default:
        break;
    }

}