#ifndef BINARY_H
#define BINARY_H

#include "../base/base.h"

class binary_t {
public:
	virtual std::string name() = 0;
	virtual void load(raw_istream_t &is) = 0;
	virtual ~binary_t() {}
};

//binary_t *binary_identifier(raw_istream_t &is);

#endif
