#include <stdint.h>

#define GB_RAM_SIZE_BYTES 0x10000
#define GAME_MEM_SPACE_BEGINNING 512
#define ROM_MAX_SIZE_BYTES 0x200000 

// cpu defines 
// for flag see GBManual P.62
#define FLAG_Z 7   // zero flag
#define FLAG_N 6   // subtract flag
#define FLAG_H 5   // half carry flag
#define FLAG_C 4   // carry flag


#define IS_LITTLE_ENDIAN *(uint16_t *)"\0\xff" < 0x100
#ifdef IS_LITTLE_ENDIAN
#define LE16TOBE16(value) ((value) >> 8) | ((value) << 8)
#else
#define LE16TOBE16(value) value
#endif