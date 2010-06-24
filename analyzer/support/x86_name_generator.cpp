#include "x86_name_generator.h"

#include "procs.h"

const char *x86_16_name_generator_t::get_proc_name(x86_16_address_t addr)
{
	uint32 ea = addr.ea();
	procs_t::const_iterator pi = procs->get_proc(ea);

	if (pi == procs->end() || !pi->name)
		return 0;

	if (ea == pi->begin)
		return pi->name;

	static char name[64];
	sprintf(name, "%s+%x (%04x:%04x)", pi->name, ea - pi->begin, addr.seg, addr.ofs);
	return name;
}
