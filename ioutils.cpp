#include "ioutils.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>

uint32 inline swap32(uint32 a) {
	return ((a >> 24) & 0x000000FF) |
	       ((a >>  8) & 0x0000FF00) |
	       ((a <<  8) & 0x00FF0000) |
	       ((a << 24) & 0xFF000000);
}

uint16 inline swap16(uint16 a) {
	return ((a >> 8) & 0x00FF) |
	       ((a << 8) & 0xFF00);
}

#if BYTE_ORDER == BIG_ENDIAN
uint32 inline letoh32(uint32 a) {
	return swap32(a);
}

uint32 inline betoh32(uint32 a) {
	return a;
}

uint16 inline letoh16(uint16 a) {
	return swap16(a)
}

uint16 inline betoh16(uint16 a) {
	return a;
}
#elif BYTE_ORDER == LITTLE_ENDIAN
uint32 inline letoh32(uint32 a) {
	return a;
}

uint32 inline betoh32(uint32 a) {
	return swap32(a);
}

uint16 inline letoh16(uint16 a) {
	return a;
}

uint16 inline betoh16(uint16 a) {
	return swap16(a);
}
#else
#error Unknown endianess!
#endif

/* read uint16 in big endian order (BE) */
uint16 freadbe16(FILE *f) {
	size_t r;
	uint16 a;
	r = fread(&a, sizeof(uint16), 1, f);
	assert(r);
	return betoh16(a);
}

/* read uint16 in little endian order (LE) */
uint16 freadle16(FILE *f) {
	size_t r;
	uint16 a;
	r = fread(&a, sizeof(uint16), 1, f);
	assert(r);
	return letoh16(a);
}

uint32 freadbe32(FILE *f) {
	size_t r;
	uint32 a;
	r = fread(&a, sizeof(uint32), 1, f);
	assert(r);
	return betoh32(a);
}

uint32 freadle32(FILE *f) {
	size_t r;
	uint32 a;
	r = fread(&a, sizeof(uint32), 1, f);
	assert(r);
	return letoh32(a);
}

uint16 readbe16(byte *p) {
	uint16 a;
	a = *(p+0) << 8 |
	    *(p+1) << 0;
	return a;
}

uint32 readbe32(byte *p) {
	uint32 a;
	a = *(p+0) << 24 |
	    *(p+1) << 16 |
	    *(p+2) <<  8 |
	    *(p+3) <<  0;
	return a;
}

uint16 readle16(byte *p) {
	uint16 a;
	a = *(p+0) << 0 |
	    *(p+1) << 8;
	return a;
}

uint32 readle32(byte *p) {
	uint32 a;
	a = *(p+0) <<  0 |
	    *(p+1) <<  8 |
	    *(p+2) << 16 |
	    *(p+3) << 24;
	return a;
}

std::string aprintf(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int n = vsnprintf(NULL, 0, format, ap);
	va_end(ap);

	char s[n+1];

	va_start(ap, format);
	n = vsnprintf(s, n+1, format, ap);
	va_end(ap);

	return std::string(s, s+n+1);
}
