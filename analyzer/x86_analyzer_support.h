#ifndef X86_ANALYZER_SUPPORT_H
#define X86_ANALYZER_SUPPORT_H

#include "../base/base.h"
#include "../x86/x86_disasm.h"

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

class x86_16_attributed_memory_t
{
	enum {
		X86_16_ATTR_OP   = 1, // Marks a byte that starts an opcde
		X86_16_ATTR_CONT = 2, // Marks all subsequent bytes that make up an opcde
		X86_16_ATTR_FLOW = 4, // Marks all ops following a non-block stop op
		X86_16_ATTR_DATA = 8,
		X86_16_ATTR_PROC = 16
	};
	struct memory_block_t
	{
		x86_16_seg_t seg;
		uint32       size;
		byte        *memory;
		byte        *attribs;

		memory_block_t()
		{}

		memory_block_t(x86_16_seg_t seg, uint32 size)
			: seg(seg), size(size)
		{}
	};

	typedef std::list<memory_block_t> memory_blocks_t;

	memory_blocks_t memory_blocks;
	const memory_block_t *get_block(x86_16_address_t addr) const;
	byte *attrib_ref_at(x86_16_address_t addr) const;
public:
	void allocate(x86_16_seg_t seg, uint32 size);
	x86_16_seg_t get_alloc_seg(byte *p);

	byte *ref_at(x86_16_address_t addr) const;

	void mark_as_code(x86_16_address_t addr, uint len);
	void mark_as_flow(x86_16_address_t addr);
	void mark_as_proc(x86_16_address_t addr);

	bool is_unmarked(x86_16_address_t addr, uint len = 1) const;
	bool is_code(x86_16_address_t addr) const;
	bool is_cont(x86_16_address_t addr) const;
	bool is_flow(x86_16_address_t addr) const;
	bool is_proc(x86_16_address_t addr) const;
};

bool x86_16_is_block_stop_op(const x86_insn &insn);
bool x86_16_is_branch(const x86_insn &insn);
bool x86_16_branch_destination(const x86_insn &insn, x86_16_address_t addr, x86_16_address_t *dst);

#endif
