#include "x86_analyzer_support.h"

void x86_16_segments_t::make_segment(x86_16_seg_t seg)
{
	x86_16_segment_t segment;
	segment.seg = seg;
	segment.min_ofs = 0xffff;
	segment.max_ofs = 0x0000;

	segments_t::iterator i = std::lower_bound(segments.begin(), segments.end(), segment);

	if (i->seg == segment.seg)
		return;

	segments.insert(i, segment);
}

void x86_16_segments_t::register_address(x86_16_address_t addr)
{
	segments_t::iterator i;
	for (i = segments.begin(); i != segments.end(); ++i)
		if (i->seg == addr.seg)
			break;

	if (i == segments.end())
		return;

	i->min_ofs = std::min(i->min_ofs, addr.ofs);
	i->max_ofs = std::max(i->max_ofs, addr.ofs);
}

x86_16_address_t x86_16_segments_t::addr_at_ea(uint32 ea) const
{
	x86_16_seg_t seg = 0;
	for (segments_t::const_iterator i = segments.begin(); i != segments.end(); ++i)
	{
		x86_16_seg_t i_seg = i->seg;
		if (i_seg << 4 > ea)
			break;
		seg = i_seg;
	}

	return x86_16_address_t(seg, ea - (seg << 4));
}

void x86_16_segments_t::dump()
{
	puts("Segments:");
	for (segments_t::const_iterator i = segments.begin(); i != segments.end(); ++i)
		printf("%04x\n", i->seg);
	putchar('\n');
}

void x86_16_attributed_memory_t::allocate(x86_16_seg_t seg, uint32 size)
{
	memory_block_t memory_block(seg, size);

	memory_block.memory  = new byte[size];
	memory_block.attribs = new byte[size];

	memset(memory_block.memory,  0, size);
	memset(memory_block.attribs, 0, size);

	memory_blocks.push_back(memory_block);
}

const x86_16_attributed_memory_t::memory_block_t *x86_16_attributed_memory_t::get_block(x86_16_address_t addr) const
{
	uint32 ea = addr.ea();

	memory_blocks_t::const_iterator i;
	for (i = memory_blocks.begin(); i != memory_blocks.end(); ++i)
		if ((uint32)(i->seg << 4) <= ea && ea < (i->seg << 4) + i->size)
			break;

	if (i == memory_blocks.end())
		return 0;

	return &*i;
}

byte *x86_16_attributed_memory_t::ref_at(x86_16_address_t addr) const
{
	const memory_block_t *b = get_block(addr);

	if (!b)
		return 0;

	uint32 offset = addr.ea() - (b->seg << 4);

	return b->memory + offset;
}

byte *x86_16_attributed_memory_t::attrib_ref_at(x86_16_address_t addr) const
{
	const memory_block_t *b = get_block(addr);

	if (!b)
		return 0;

	uint32 offset = addr.ea() - (b->seg << 4);

	return b->attribs + offset;
}

void x86_16_attributed_memory_t::mark_as_code(x86_16_address_t addr, uint len)
{
	byte *p = attrib_ref_at(addr);
	*p++ = X64_16_ATTR_CODE;
	while (--len)
		*p++ = X64_16_ATTR_CONT;
}

bool x86_16_attributed_memory_t::is_unmarked(x86_16_address_t addr, uint len) const
{
	byte *p = attrib_ref_at(addr);
	for (; len; ++p, --len)
		if (*p)
			return false;

	return true;
}

bool x86_16_attributed_memory_t::is_code(x86_16_address_t addr) const
{
	byte *p = attrib_ref_at(addr);
	return (*p) & X64_16_ATTR_CODE;
}

bool x86_16_is_block_stop_op(const x86_insn &insn)
{
	return insn.op_name == op_jmp
	    || insn.op_name == op_ret
	    || insn.op_name == op_retf
	    || insn.op_name == op_iret;
}

bool x86_16_is_branch(const x86_insn &insn)
{
	switch (insn.op_name)
	{
	case op_call:
	case op_ja:
	case op_jbe:
	case op_jc:
	case op_jcxz:
	case op_je:
	case op_jg:
	case op_jge:
	case op_jl:
	case op_jle:
	case op_jmp:
	case op_jnc:
	case op_jne:
	case op_jno:
	case op_jns:
	case op_jo:
	case op_jpe:
	case op_jpo:
	case op_js:
		return true;
	default:;
	}

	return false;
}
