#ifndef ENDIAN_H
#define ENDIAN_H

#include <sys/param.h>

#include "types.h"

inline
uint16 swap16(uint16 a) {
	return ((a >> 8) & 0x00FF) |
	       ((a << 8) & 0xFF00);
}

inline
uint32 swap32(uint32 a) {
	return ((a >> 24) & 0x000000FF) |
	       ((a >>  8) & 0x0000FF00) |
	       ((a <<  8) & 0x00FF0000) |
	       ((a << 24) & 0xFF000000);
}

#if BYTE_ORDER == BIG_ENDIAN
inline
uint16 letoh16(uint16 a) {
	return swap16(a);
}

inline
uint16 betoh16(uint16 a) {
	return a;
}

inline
uint32 letoh32(uint32 a) {
	return swap32(a);
}

inline
uint32 betoh32(uint32 a) {
	return a;
}

inline
uint16 htole16(uint16 a) {
	return swap16(a);
}

inline
uint16 htobe16(uint16 a) {
	return a;
}

inline
uint32 htole32(uint32 a) {
	return swap32(a);
}

inline
uint32 htobe32(uint32 a) {
	return a;
}
#elif BYTE_ORDER == LITTLE_ENDIAN
inline
uint16 letoh16(uint16 a) {
	return a;
}

inline
uint16 betoh16(uint16 a) {
	return swap16(a);
}

inline
uint32 letoh32(uint32 a) {
	return a;
}

inline
uint32 betoh32(uint32 a) {
	return swap32(a);
}

inline
uint16 htole16(uint16 a) {
	return a;
}

inline
uint16 htobe16(uint16 a) {
	return swap16(a);
}

inline
uint32 htole32(uint32 a) {
	return a;
}

inline
uint32 htobe32(uint32 a) {
	return swap32(a);
}

#else
#error Unknown endianess!
#endif

#endif
