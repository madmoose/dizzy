#ifndef RAW_STREAM_H
#define RAW_STREAM_H

#include "types.h"
#include "endian.h"

#include <string.h>
#include <string>

/*
 * This header defines the following classes:
 *
 * raw_istream_t  - a virtual base class for input streams
 * raw_ifstream_t - a class for reading from files
 * raw_imstream_t - a class for reading from a piece of memory
 *
 * raw_ostream_t  - a virtual base class for output streams
 * raw_omstream_t - a class for writing to a piece of memory
 *
 * For reading from a file, it's probably preferable to read the entire file
 * into memory using raw_imstream_t - there's a constructor that takes a filename.
 *
 */

class raw_istream_t {
protected:
	uint32 _size;
	uint32 _pos;
public:
	raw_istream_t()
		: _size(0), _pos(0)
	{}
	virtual ~raw_istream_t()
	{}

	virtual void read(byte *p, uint s) = 0;

	byte   readbyte();

	uint16 readle16();
	uint16 readbe16();

	uint32 readle32();
	uint32 readbe32();

	uint16 readaheadbe16();

	uint32 size();
	uint32 pos();
	uint32 rem();

	virtual void seek_set(uint p);
	void seek_cur(int d);

	void reset();
private:
	raw_istream_t(const raw_istream_t&);
	const raw_istream_t& operator=(const raw_istream_t&);
};

class raw_ifstream_t : public raw_istream_t {
	int _fd;
public:
	raw_ifstream_t(const std::string &filename);
	~raw_ifstream_t();

	void read(byte *p, uint s);
	void seek_set(uint p);
};

#define DELETE_WHEN_DONE true

class raw_imstream_t : public raw_istream_t {
	byte *_p;
	bool  _delete_when_done;
public:
	raw_imstream_t(byte *p, uint32 s, bool delete_when_done = false);
	raw_imstream_t(const std::string &filename);
	~raw_imstream_t();

	void read(byte *p, uint s);
private:
	raw_imstream_t(const raw_imstream_t&);
	const raw_imstream_t& operator=(const raw_imstream_t&);
};

class raw_ostream_t {
protected:
	uint32  _size;
	uint32  _pos;
public:
	raw_ostream_t()
		: _size(0), _pos(0)
	{}
	virtual ~raw_ostream_t()
	{}

	virtual void write(const byte *p, uint s) = 0;

	void writebyte(byte a);

	void writele16(uint16 a);
	void writebe16(uint16 a);

	void writele32(uint32 a);
	void writebe32(uint32 a);

	uint32 size();
	uint32 pos();
	uint32 rem();

	void seek_set(uint p);
	void seek_cur(int  d);
};

class raw_omstream_t : public raw_ostream_t {
protected:
	byte   *_p;
	bool    _delete_when_done;
	bool    _expanding;
	uint32  _capacity;
public:
	raw_omstream_t(byte *p, uint32 s, bool delete_when_done = false);
	raw_omstream_t(uint32 size);
	raw_omstream_t();
	~raw_omstream_t();

	void write(const byte *p, uint s);
private:
	raw_omstream_t(const raw_omstream_t&);
	const raw_omstream_t& operator=(const raw_omstream_t&);
};

/*
 * raw_istream_t inline classes
 */

inline
byte raw_istream_t::readbyte() {
	byte a;
	read(&a, sizeof(a));
	return a;
}

inline
uint16 raw_istream_t::readle16() {
	uint16 a;
	read((byte*)&a, sizeof(a));
	return letoh16(a);
}

inline
uint16 raw_istream_t::readbe16() {
	uint16 a;
	read((byte*)&a, sizeof(a));
	return betoh16(a);
}

inline
uint32 raw_istream_t::readle32() {
	uint32 a;
	read((byte*)&a, sizeof(uint32));
	return letoh32(a);
}

inline
uint32 raw_istream_t::readbe32() {
	uint32 a;
	read((byte*)&a, sizeof(a));
	return betoh32(a);
}

inline
uint16 raw_istream_t::readaheadbe16() {
	uint16 a = readbe16();
	seek_cur(-2);
	return a;
}

inline
uint32 raw_istream_t::size() {
	return _size;
}

inline
uint32 raw_istream_t::pos() {
	return _pos;
}

inline
uint32 raw_istream_t::rem() {
	return size() - pos();
}

inline
void raw_istream_t::seek_set(uint p)
{
	_pos = p;
}

inline
void raw_istream_t::seek_cur(int d) {
	seek_set(pos() + d);
}

inline
void raw_istream_t::reset() {
	seek_set(0);
}

/*
 * raw_imstream_t inline classes
 */

inline
void raw_imstream_t::read(byte *p, uint s)
{
	memcpy(p, _p + _pos, s);
	_pos += s;
}

/*
 * raw_ostream_t inline methods
 */

inline
uint32 raw_ostream_t::size()
{
	return _size;
}

inline
uint32 raw_ostream_t::pos()
{
	return _pos;
}

inline
uint32 raw_ostream_t::rem()
{
	return size() - pos();
}

inline
void raw_ostream_t::seek_set(uint p)
{
	_pos = p;
}

inline
void raw_ostream_t::seek_cur(int d) {
	seek_set(pos() + d);
}

#endif
