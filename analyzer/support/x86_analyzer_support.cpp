#include "x86_analyzer_support.h"
#include "x86/x86_disasm.h"

#include <stdio.h>

void x86_16_segments_t::make_segment(x86_16_seg_t seg)
{
	x86_16_segment_t segment;
	segment.seg = seg;
	segment.min_ofs = 0xffff;
	segment.max_ofs = 0x0000;

	segments_t::iterator i = std::lower_bound(segments.begin(), segments.end(), segment);

	if (i != segments.end() && i->seg == segment.seg)
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
		if (((uint32)i_seg) << 4 > ea)
			break;
		seg = i_seg;
	}

	return x86_16_address_t(seg, ea - (seg << 4));
}

void x86_16_segments_t::dump()
{
	puts("Segments:");
	for (segments_t::const_iterator i = segments.begin(); i != segments.end(); ++i)
		printf("%04x: %06x-%06x\n", i->seg, i->min_ofs, i->max_ofs);
	putchar('\n');
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

bool x86_16_branch_destination(const x86_insn &insn, x86_16_address_t addr, x86_16_address_t *dst)
{
	if (insn.arg[0].kind == KN_ADR)
		*dst = x86_16_address_t(insn.arg[0].seg, insn.arg[0].ofs);
	else
	if (insn.arg[0].kind == KN_IMM && insn.arg[0].size == SZ_BYTE)
		*dst = x86_16_address_t(addr.seg, addr.ofs + insn.op_size + (int8)insn.arg[0].imm);
	else
	if (insn.arg[0].kind == KN_IMM && insn.arg[0].size == SZ_WORD)
		*dst = x86_16_address_t(addr.seg, addr.ofs + insn.op_size + (int16)insn.arg[0].imm);
	else
		return false;

	return true;
}
