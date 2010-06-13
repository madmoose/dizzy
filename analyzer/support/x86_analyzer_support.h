#ifndef X86_ANALYZER_SUPPORT_H
#define X86_ANALYZER_SUPPORT_H

#include "base/base.h"
#include "x86/x86_disasm.h"

#include <list>

/*
 * x86_16_address_t
 * x86_16_segment_t
 * x86_16_segments_t
 * x86_16_attributed_memory_t
 */


typedef uint16 x86_16_seg_t;
typedef uint16 x86_16_ofs_t;

struct x86_16_address_t
{
	x86_16_seg_t seg;
	x86_16_ofs_t ofs;

	x86_16_address_t()
		: seg(0), ofs(0)
	{}

	x86_16_address_t(x86_16_seg_t seg, x86_16_ofs_t ofs)
		: seg(seg), ofs(ofs)
	{}

	uint32 ea() const { return (seg << 4) + ofs; }
};

inline
bool operator<(const x86_16_address_t &a, const x86_16_address_t &b)
{
	return a.ea() < b.ea();
}

struct x86_16_segment_t {
	x86_16_seg_t seg;
	uint16       min_ofs;
	uint16       max_ofs;
};

inline
bool operator<(const x86_16_segment_t &a, const x86_16_segment_t &b)
{
	return a.seg < b.seg;
}

class x86_16_segments_t
{
	typedef std::list<x86_16_segment_t> segments_t;
	segments_t segments;
public:
	void make_segment(x86_16_seg_t seg);
	void register_address(x86_16_address_t addr);

	x86_16_address_t addr_at_ea(uint32 ea) const;

	void dump();
};

bool x86_16_is_block_stop_op(const x86_insn &insn);
bool x86_16_is_branch(const x86_insn &insn);
bool x86_16_branch_destination(const x86_insn &insn, x86_16_address_t addr, x86_16_address_t *dst);

#endif
