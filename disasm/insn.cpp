#include "insn.h"

void insn_t::add_arg(insn_arg_t *arg)
{
	insn_arg_t **p = &args;

	while (*p)
		p = &((*p)->next);

	*p = arg;
}
