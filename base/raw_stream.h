#ifndef RAW_STREAM_H
#define RAW_STREAM_H

#include "types.h"
#include "endian.h"

#include <string>

/*
 * This header defines the following classes:
 *
 * raw_istream  - a virtual base class for input streams
 * raw_ifstream - a class for reading from files
 * raw_imstream - a class for reading from a piece of memory
 *
 * raw_ostream  - a virtual base class for output streams
 * raw_omstream - a class for writing to a piece of memory
 *
 * For reading from a file, it's probably preferable to read the entire file
 * into memory using raw_imstream - there's a constructor that takes a filename.
 *
 */

class raw_istream {
protected:
	uint32 _size;
	uint32 _pos;
public:
	raw_istream()
	{}
	virtual ~raw_istream()
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
};

class raw_ifstream : public raw_istream {
	int _fd;
public:
	raw_ifstream(const std::string &filename);
	~raw_ifstream();

	void read(byte *p, uint s);
	void seek_set(uint p);
};

#define DELETE_WHEN_DONE true

class raw_imstream : public raw_istream {
	byte *_p;
	bool  _delete_when_done;
public:
	raw_imstream(byte *p, uint32 s, bool delete_when_done = false);
	raw_imstream(const std::string &filename);
	~raw_imstream();

	void read(byte *p, uint s);
};

class raw_ostream {
protected:
	uint32  _size;
	uint32  _pos;
public:
	raw_ostream()
	{}
	virtual ~raw_ostream()
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

class raw_omstream : public raw_ostream {
protected:
	byte   *_p;
	bool    _delete_when_done;
	bool    _expanding;
	uint32  _capacity;
public:
	raw_omstream(byte *p, uint32 s, bool delete_when_done = false);
	raw_omstream(uint32 size);
	raw_omstream();
	~raw_omstream();

	void write(const byte *p, uint s);
};

/*
 * raw_istream inline classes
 */

inline
byte raw_istream::readbyte() {
	byte a;
	read(&a, sizeof(a));
	return a;
}

inline
uint16 raw_istream::readle16() {
	uint16 a;
	read((byte*)&a, sizeof(a));
	return letoh16(a);
}

inline
uint16 raw_istream::readbe16() {
	uint16 a;
	read((byte*)&a, sizeof(a));
	return betoh16(a);
}

inline
uint32 raw_istream::readle32() {
	uint32 a;
	read((byte*)&a, sizeof(uint32));
	return letoh32(a);
}

inline
uint32 raw_istream::readbe32() {
	uint32 a;
	read((byte*)&a, sizeof(a));
	return betoh32(a);
}

inline
uint16 raw_istream::readaheadbe16() {
	uint16 a = readbe16();
	seek_cur(-2);
	return a;
}

inline
uint32 raw_istream::size() {
	return _size;
}

inline
uint32 raw_istream::pos() {
	return _pos;
}

inline
uint32 raw_istream::rem() {
	return size() - pos();
}

inline
void raw_istream::seek_set(uint p)
{
	_pos = p;
}

inline
void raw_istream::seek_cur(int d) {
	seek_set(pos() + d);
}

inline
void raw_istream::reset() {
	seek_set(0);
}

/*
 * raw_imstream inline classes
 */

inline
void raw_imstream::read(byte *p, uint s)
{
	memcpy(p, _p + _pos, s);
	_pos += s;
}

/*
 * raw_ostream inline methods
 */

inline
uint32 raw_ostream::size()
{
	return _size;
}

inline
uint32 raw_ostream::pos()
{
	return _pos;
}

inline
uint32 raw_ostream::rem()
{
	return size() - pos();
}

inline
void raw_ostream::seek_set(uint p)
{
	_pos = p;
}

inline
void raw_ostream::seek_cur(int d) {
	seek_set(pos() + d);
}

#endif
