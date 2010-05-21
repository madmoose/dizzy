#ifndef BINARY_H
#define BINARY_H

#include "../base/base.h"
#include "../memory.h"

class binary_loader_t {
	uint32   _base;
	memory_t _load_image;
protected:
	void set_base(uint32 base);
	void set_load_image(memory_t &load_image);
public:
	virtual std::string name() = 0;
	virtual void load(raw_istream_t &is, uint32 base) = 0;

	uint32   base();
	memory_t load_image();
};

binary_loader_t *binary_identifier(raw_istream_t &is);

#endif
