#ifndef IOUTILS_H
#define IOUTILS_H

#include <stdio.h>

typedef unsigned char  byte;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef signed char  int8;
typedef signed short int16;
typedef signed long  int32;

#ifndef HAS_UINT
typedef unsigned int uint;
#endif

uint32 inline letoh32(uint32 a);
uint32 inline betoh32(uint32 a);
uint16 inline letoh16(uint16 a);
uint16 inline betoh16(uint16 a);

uint16 freadbe16(FILE *f);
uint16 freadle16(FILE *f);
uint32 freadbe32(FILE *f);
uint32 freadle32(FILE *f);
uint16 readbe16(byte *p);
uint32 readbe32(byte *p);
uint16 readle16(byte *p);
uint32 readle32(byte *p);

#endif
