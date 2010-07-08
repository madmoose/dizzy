#include "attributed_memory.h"

void attributed_memory_t::allocate(uint32 begin, uint32 size)
{
	memory_block_t memory_block(begin, size);

	memory_block.memory  = new byte[size];
	memory_block.attribs = new byte[size];

	memset(memory_block.memory,  0, size);
	memset(memory_block.attribs, 0, size);

	memory_blocks.push_back(memory_block);
}

const attributed_memory_t::memory_block_t *attributed_memory_t::get_block(uint32 ea) const
{
	memory_blocks_t::const_iterator i;
	for (i = memory_blocks.begin(); i != memory_blocks.end(); ++i)
	{
		if (i->begin <= ea && ea < i->begin + i->size)
			break;
	}

	if (i == memory_blocks.end())
		return 0;

	return &*i;
}

byte *attributed_memory_t::ref_at(uint32 ea) const
{
	const memory_block_t *b = get_block(ea);

	if (!b)
		return 0;

	return b->memory + (ea - b->begin);
}

byte *attributed_memory_t::attrib_ref_at(uint32 ea) const
{
	const memory_block_t *b = get_block(ea);

	if (!b)
		return 0;

	return b->attribs + (ea - b->begin);
}

void attributed_memory_t::mark_as_code(uint32 ea, uint len)
{
	byte *p = attrib_ref_at(ea);
	*p++ |= ATTR_OP;
	while (--len)
		*p++ |= ATTR_CONT;
}

void attributed_memory_t::mark_as_flow(uint32 ea)
{
	byte *p = attrib_ref_at(ea);
	*p |= ATTR_FLOW;
}

void attributed_memory_t::mark_as_proc(uint32 ea)
{
	byte *p = attrib_ref_at(ea);
	*p |= ATTR_PROC;
}

void attributed_memory_t::mark_as_align(uint32 ea)
{
	byte *p = attrib_ref_at(ea);
	*p |= ATTR_ALIGN;
}

void attributed_memory_t::unmark_as_proc(uint32 ea)
{
	byte *p = attrib_ref_at(ea);
	*p &= ~ATTR_PROC;
}

bool attributed_memory_t::is_unmarked(uint32 ea, uint len) const
{
	byte *p = attrib_ref_at(ea);
	for (; len; ++p, --len)
		if (*p)
			return false;

	return true;
}

bool attributed_memory_t::is_code(uint32 ea) const
{
	byte *p = attrib_ref_at(ea);
	return (*p) & (ATTR_OP | ATTR_CONT);
}

bool attributed_memory_t::is_op(uint32 ea) const
{
	byte *p = attrib_ref_at(ea);
	return (*p) & ATTR_OP;
}

bool attributed_memory_t::is_cont(uint32 ea) const
{
	byte *p = attrib_ref_at(ea);
	return (*p) & ATTR_CONT;
}

bool attributed_memory_t::is_flow(uint32 ea) const
{
	byte *p = attrib_ref_at(ea);
	return (*p) & ATTR_FLOW;
}

bool attributed_memory_t::is_proc(uint32 ea) const
{
	byte *p = attrib_ref_at(ea);
	return (*p) & ATTR_PROC;
}

bool attributed_memory_t::is_align(uint32 ea) const
{
	byte *p = attrib_ref_at(ea);
	return (*p) & ATTR_ALIGN;
}
