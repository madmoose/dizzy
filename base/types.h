#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint8_t  byte;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;

typedef unsigned int uint;

inline
byte lo(uint16 v) { return (byte)(v & 0xff); }

inline
byte hi(uint16 v) { return (byte)(v >> 8); }

#endif
