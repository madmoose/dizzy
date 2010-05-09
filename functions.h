#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <set>

#include "segofs.h"

struct functions_t
{
	std::set<segofs_t> call_destinations;

	void add_call_destination(segofs_t s);
};

#endif
