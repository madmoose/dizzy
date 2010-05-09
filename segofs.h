#ifndef SEGOFS_H
#define SEGOFS_H

#include "ioutils.h"

class segofs_t {
public:
	uint16 seg;
	uint16 ofs;

	uint32 addr() const { return (seg << 4) + ofs; }

	segofs_t &operator+=(int d)
	{
		uint32 n = ofs + d;

		ofs = n & 0x0000ffff;

		return *this;
	}

	segofs_t() {}
	segofs_t(uint16 aseg, uint16 aofs) :
		seg(aseg), ofs(aofs) {}
};

inline
bool operator<(const segofs_t &a, const segofs_t &b)
{
	return a.addr() < b.addr();
}

#endif
