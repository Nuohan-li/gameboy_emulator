#include <stdint.h>

// memory defines
#define GB_RAM_SIZE_BYTES 0x10000
#define GAME_MEM_SPACE_BEGINNING 512
#define ROM_MAX_SIZE_BYTES 0x200000 
#define NUM_BANKING_MODES 26

// timer 
#define TIMER_ADDR 0xFF05
#define TIMER_RESET_VALUE_ADDR 0xFF06
#define TIMER_CONTROLLER_ADDR 0xFF07 

// cpu defines 
// for flag see GBManual P.62
#define FLAG_Z 7   // zero flag
#define FLAG_N 6   // subtract flag
#define FLAG_H 5   // half carry flag
#define FLAG_C 4   // carry flag

// change from little endian to big endian
#define IS_LITTLE_ENDIAN *(uint16_t *)"\0\xff" < 0x100
#ifdef IS_LITTLE_ENDIAN
#define LE16TOBE16(value) ((value) >> 8) | ((value) << 8)
#else
#define LE16TOBE16(value) value
#endif