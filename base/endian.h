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

inline
uint16 readle16(byte *p)
{
	return p[0] + (p[1] << 8);
}

inline
uint16 readbe16(byte *p)
{
	return p[1] + (p[0] << 8);
}

inline
uint32 readle32(byte *p)
{
	return p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
}

inline
uint32 readbe32(byte *p)
{
	return p[3] + (p[2] << 8) + (p[1] << 16) + (p[0] << 24);
}

inline
void writele16(byte *p, uint16 a)
{
	p[0] = (a >>  0) & 0xff;
	p[1] = (a >>  8) & 0xff;
}

inline
void writebe16(byte *p, uint16 a)
{
	p[1] = (a >>  0) & 0xff;
	p[0] = (a >>  8) & 0xff;
}

inline
void writele32(byte *p, uint32 a)
{
	p[0] = (a >>  0) & 0xff;
	p[1] = (a >>  8) & 0xff;
	p[2] = (a >> 16) & 0xff;
	p[3] = (a >> 24) & 0xff;
}

inline
void writebe32(byte *p, uint32 a)
{
	p[3] = (a >>  0) & 0xff;
	p[2] = (a >>  8) & 0xff;
	p[1] = (a >> 16) & 0xff;
	p[0] = (a >> 24) & 0xff;
}

#endif
