#ifndef BINARY_H
#define BINARY_H

#include "../base/base.h"

struct binary_loader {
	virtual std::string name() = 0;
	virtual void load(raw_istream &is) = 0;
};

binary_loader *binary_identifier(raw_istream &is);

#endif
