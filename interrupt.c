#include "Interrupt.h"
#include "memory.h"

void request_interrupt(cpu  *cpu_ctx, int bit_position){
    switch (bit_position){
        case VBLANK:
            write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00000001);
            break;
        case LCD:
            write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00000010);
            break;
        case TIMER:
            write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00000100);
            break;
        case JOYPAD:
            write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0b00010000);
        default:
            break;
    }

}
void service_interrupt(cpu *cpu_ctx, int interrupt){
    cpu_ctx->interrupt_master_switch = false;

    // reset interrupt request register
    write_byte(cpu_ctx->memory, INTERRUPT_REQ_REG, 0);

    // push PC onto stack
    push_two_bytes(cpu_ctx, cpu_ctx->PC.value);

    // set PC to correct routine
    switch(interrupt){
        case VBLANK:
            cpu_ctx->PC.value = VBLANK_ROUTINE;
            break;
        case LCD:
            cpu_ctx->PC.value = LCD_ROUTINE;
            break;
        case TIMER:
            cpu_ctx->PC.value = TIMER_ROUTINE;
            break;
        case JOYPAD:
            cpu_ctx->PC.value = JOYPAD_ROUTINE;
            break;
        default:
            break;
    }
}


void check_interrupt(cpu *cpu_ctx){
    if(cpu_ctx->interrupt_master_switch){
        uint8_t request = read_one_byte(cpu_ctx->memory, INTERRUPT_REQ_REG);
        uint8_t interrupt_enable = read_one_byte(cpu_ctx->memory, INTERRUPT_ENABLE);

        // check if the particular interrupt is enabled
        if(request > 0){
            // vblank
            if(request & 0b00000001){
                if(interrupt_enable & 0b00000001){
                    service_interrupt(cpu_ctx, VBLANK);
                }
            }
            // LCD
            else if(request & 0b00000010){
                if(interrupt_enable & 0b00000010){
                    service_interrupt(cpu_ctx, LCD);
                }
            }
            // timer
            else if(request & 0b00000100){
                if(interrupt_enable & 0b00000100){
                    service_interrupt(cpu_ctx, TIMER);
                }
            }
            // joypad
            else if(request & 0b00010000){
                if(interrupt_enable & 0b00010000){
                    service_interrupt(cpu_ctx, JOYPAD);
                }
            }
        }
    }
}