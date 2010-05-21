#ifndef DATA_H
#define DATA_H

#include "types.h"

class data_t
{
	byte *_p;
public:
	data_t()
		: _p(0)
	{}

	data_t(uint size)
		: _p(new byte[size])
	{
		//puts("Constructor 1");
		if (!_p) return;
		_p[-1] = 0;
	}

	data_t(const data_t &b)
		: _p(b._p)
	{
		//puts("Constructor 2");
		if (!_p) return;
		++_p[-1];
	}

	~data_t()
	{
		//puts("Destructor");
		if (!_p) return;
		if (--_p[-1])
			delete[] _p;
	}

	byte *p() const
	{
		return _p;
	}

	byte &operator[](uint i)
	{
		return _p[i];
	}

	byte operator[](uint i) const
	{
		return _p[i];
	}
};

#endif
