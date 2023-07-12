#include <stdint.h>



// change from little endian to big endian
#define IS_LITTLE_ENDIAN *(uint16_t *)"\0\xff" < 0x100
#ifdef IS_LITTLE_ENDIAN
#define LE16TOBE16(value) ((value) >> 8) | ((value) << 8)
#else
#define LE16TOBE16(value) value
#endif