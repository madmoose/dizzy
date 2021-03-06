#ifndef PROCS_H
#define PROCS_H

#include "base/base.h"
#include "x86_analyzer_support.h"

#include <set>


struct proc_t : public range_t<uint32>
{
	x86_16_address_t addr;

	const char *name;

	std::set<uint32> blocks;

	proc_t()
		: addr(), name(0), blocks()
	{}
};

struct procs_t : range_set_t<proc_t>
{
};

#endif
