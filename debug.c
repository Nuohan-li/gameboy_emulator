#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "memory.h"

// dump the content of the memory 
void dump_memory(uint8_t *memory, size_t size_byte){
    uint16_t bytes_counter = 0;
    int num_of_lines = 0;
    int num_bytes_to_print = 0;
    int remainder = size_byte % 32;
    if(remainder == 0){
        num_of_lines = size_byte / 32;
    } else{
        num_of_lines = size_byte / 32 + 1;
    }
    for(int i = 0; i < num_of_lines; i++){
        printf("%04u     ", bytes_counter);
        if(i == num_of_lines - 1 && remainder != 0){ 
            num_bytes_to_print = remainder;
        } else{
            num_bytes_to_print = 32;
        }
        // printing the hex value 
        for(int j = 0; j < num_bytes_to_print; j++){
            printf("%02X ", memory[(i * num_bytes_to_print) + j]);
            if(j == 15){
                printf(" ");
            }
        }
        printf(" ");
        // upon reaching last line, add padding to hex values so that all characters will align
        if(i == num_of_lines - 1){
            int padding = 32 - num_bytes_to_print;
            for (int j = 0; j < padding; j++) {
                printf("   ");
            }
        }
        // printing characters
        for (int j = 0; j < num_bytes_to_print; j++) {
            uint8_t value = memory[i * 32 + j];
            if (isprint(value)) {
                printf("%c", value);
            } else {
                printf(".");
            }
        }
        printf("\n");
        bytes_counter += num_bytes_to_print;
    }
}

void dump_register_content(cpu *cpu_ctx){
    
}

void dump_game_content(char *file_name){
    // loading game into memory space
    printf("file name is: %s\n", file_name);
    // open the file in "rb" mode -> rb = read binary 
    FILE* f = fopen(file_name, "rb");  
    // fseek sets the file pointer to the position "SEEK_END", which is the end of the file
    fseek(f, 0, SEEK_END);
    // ftell returns the current position of the file pointer. It corresponds to the size of the file in bytes because 
    // the file pointer is currently pointing at the end of the file
    long size = ftell(f);
    // SEEK_SET returns the file pointer back to the beginning
    fseek(f, 0, SEEK_SET);

    uint8_t game[size];
    fread(game, size, 1, f);
    dump_memory(game, size);
}

// void debugger(cpu *cpu_ctx){
//     char *file_name = "GAMES/GAMES/CAVE.ch8";
//     FILE* f = fopen(file_name, "rb");  
//     fseek(f, 0, SEEK_END);
//     long size = ftell(f);
//     fseek(f, 0, SEEK_SET);
//     uint8_t game[size];
//     fread(game, size, 1, f);
//     init_gb_memory(&cpu_ctx->memory);
//     load_game(cpu_ctx, file_name);
//     dump_memory(cpu_ctx->memory.ram, 4096);

//     uint16_t opcode = 0;
//     char input[20];
//     int address = 512;
//     int size_to_dump = 0;
//     // char game_name[50];
//     printf("Debugging mode\n");
//     while(1){
//         printf(">>> ");
//         fgets(input, sizeof(input), stdin);
//         switch (tolower(input[0]))
//         {
//         // execute a game instruction
//         case 'e':
//             sscanf(input, "%*s %d", &address);
//             if(address < 512 || address > size + GAME_MEM_SPACE_BEGINNING){
//                 printf("You have stepped out of memory space allocated for game instructions\n");
//                 break;
//             }
//             if(address % 2 != 0){
//                 printf("Every instruction starts at even address\n");
//                 break;        
//             }
//             opcode = memory_get_two_bytes(&cpu_ctx->memory, address);
//             execute_opcode(cpu_ctx, opcode);
//             break;
//         // dump memory segments 
//         case 'd':
//             sscanf(input, "%*s %d %d", &address, &size_to_dump);
//             dump_memory(&cpu_ctx->memory.ram[address], size_to_dump);
//             break;
        
//         // dump game
//         case 'g':
//             dump_memory(&cpu_ctx->memory.ram[GAME_MEM_SPACE_BEGINNING], size);
//             break;
            
//         default:
//             printf("Invalid command\n");
//             break;
//         }
//     }
// }

void test(){
    // memory test 
    cpu cpu;
    cpu_init(&cpu);
    dump_memory(&cpu.memory->ram[0xFF05], 0xFB);
}