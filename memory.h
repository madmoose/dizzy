#ifndef MEMORY_H
#define MEMORY_H

#include "base/base.h"

class memory_t
{
	data_t _data;
	uint32 _base;
	uint32 _size;
public:
	memory_t()
		: _base(0), _size(0)
	{}

	memory_t(uint32 base, uint32 size)
		: _data(size), _base(base), _size(size)
	{}

	uint32 base()
	{
		return _base;
	}

	uint32 size()
	{
		return _size;
	}

	bool is_valid_address(uint32 ea) const
	{
		return ea >= _base && ea < _base + _size;
	}

	byte &operator[](uint32 ea)
	{
		return _data[ea - _base];
	}

	byte operator[](uint32 ea) const
	{
		return _data[ea - _base];
	}

	uint16 readle16(uint32 ea) const
	{
		byte *p = _data.p() + ea - _base;
		return ::readle16(p);
	}

	void writele16(uint32 ea, uint16 v) const
	{
		byte *p = _data.p() + ea - _base;
		::writele16(p, v);
	}
};

#endif
