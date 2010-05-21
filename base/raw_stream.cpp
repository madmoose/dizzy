#include "raw_stream.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

/*
 * raw_ifstream_t
 */

raw_ifstream_t::raw_ifstream_t(const std::string &filename)
{
	_fd = open(filename.c_str(), O_RDONLY);
	_size = lseek(_fd, 0, SEEK_END);
	_pos  = lseek(_fd, 0, SEEK_SET);
}

raw_ifstream_t::~raw_ifstream_t()
{
	close(_fd);
}

void raw_ifstream_t::read(byte *p, uint s)
{
	int r;

	do {
		r = ::read(_fd, p, s);
		if (r == -1)
			break;

		_pos += r;
		p += r;
		s -= r;
	} while (s);
}

void raw_ifstream_t::seek_set(uint p)
{
	lseek(_fd, p, SEEK_SET);
	_pos = p;
}

/*
 * raw_imstream_t
 */

raw_imstream_t::raw_imstream_t(byte *p, uint32 s, bool delete_when_done)
{
	_p = p;
	_size = s;
	_pos = 0;
	_delete_when_done = delete_when_done;
}

raw_imstream_t::raw_imstream_t(const std::string &filename)
{
	raw_ifstream_t is(filename);
	_size = is.size();
	_p = new byte[_size];
	is.read(_p, _size);
	_pos = 0;
	_delete_when_done = true;
}


raw_imstream_t::~raw_imstream_t()
{
	if (_delete_when_done)
		delete[] _p;
}

/*
 * raw_ostream_t
 */

void raw_ostream_t::writebyte(byte a)
{
	write(&a, sizeof(a));
}

void raw_ostream_t::writele16(uint16 a)
{
	a = htole16(a);
	write((byte*)&a, sizeof(a));
}

void raw_ostream_t::writebe16(uint16 a)
{
	a = htobe16(a);
	write((byte*)&a, sizeof(a));
}

void raw_ostream_t::writele32(uint32 a)
{
	a = htole32(a);
	write((byte*)&a, sizeof(a));
}

void raw_ostream_t::writebe32(uint32 a)
{
	a = htobe32(a);
	write((byte*)&a, sizeof(a));
}

/*
 * raw_ostream_tm
 */

raw_ostream_tm::raw_ostream_tm(byte *p, uint32 size, bool delete_when_done)
{
	_p = p;
	_pos = 0;
	_size = size;
	_delete_when_done = _delete_when_done;
	_expanding = false;
}

raw_ostream_tm::raw_ostream_tm(uint32 size)
{
	_p = new byte[size];
	_pos = 0;
	_size = size;
	_delete_when_done = true;
	_expanding = false;
}

raw_ostream_tm::raw_ostream_tm()
{
	_p = 0;
	_pos = 0;
	_size = 0;
	_delete_when_done = true;
	_expanding = true;
	_capacity = 0;
}

raw_ostream_tm::~raw_ostream_tm()
{
	if (_delete_when_done)
		delete[] _p;
}

void raw_ostream_tm::write(const byte *p, uint s)
{
	if (_expanding && _pos + s >= _capacity)
	{
		uint32 new_capacity = 2 * _capacity;
		while (_pos + s >= new_capacity)
			new_capacity *= 2;
		byte *new_p = new byte[new_capacity];
		memcpy(new_p, _p, _size);
		delete[] _p;
		_p = new_p;
		_capacity = new_capacity;
		_size = std::max(_size, _pos + s);
	}

	memcpy(_p + _pos, p, s);
	_pos += s;
}
