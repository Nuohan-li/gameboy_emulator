#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "cpu.h"

// dump the content of the memory 
void dump_memory(uint8_t *memory, size_t size_byte);
void dump_register_content(cpu *cpu);
void dump_game_content(char *file_name);
void dump_game_to_file(char *game, char *output);
void log_error(const char *error_msg, const char *component, bool write_to_file);
void log_trace(bool write_to_file, const char *format, ...);
void log_opcode();
void debugger(cpu *cpu_ctx);
void test();