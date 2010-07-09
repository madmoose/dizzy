#ifndef BLOCKS_H
#define BLOCKS_H

#include "base/base.h"
#include "x86_analyzer_support.h"
#include "x86/x86_disasm.h"


struct block_t : public range_t<uint32>
{
	e_ops  terminator_op_name;
};

struct blocks_t : range_set_t<block_t>
{
};

#endif
