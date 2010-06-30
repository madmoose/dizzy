#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H

#include "procs.h"

struct annotations_t
{
	procs_t    *procs;

	annotations_t()
		: procs(new procs_t)
	{}
};

#endif
