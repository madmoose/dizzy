#ifndef X86_NAME_GENERATOR_H
#define X86_NAME_GENERATOR_H

#include "base/base.h"
#include "x86_analyzer_support.h"

struct procs_t;
struct datadefs_t;

struct x86_16_name_generator_t
{
	procs_t    *procs;

	const char *get_proc_name(x86_16_address_t ea);
};

#endif
