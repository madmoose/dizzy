#ifndef ATTRIBUTED_MEMORY_H
#define ATTRIBUTED_MEMORY_H

#include "base/base.h"

#include <list>

class attributed_memory_t
{
	enum {
		ATTR_OP    = 1,  // Marks a byte that starts an opcde
		ATTR_CONT  = 2,  // Marks all subsequent bytes that make up an opcde
		ATTR_FLOW  = 4,  // Marks all ops following a non-block stop op
		ATTR_DATA  = 8,  // Mark as data
		ATTR_PROC  = 16, // Mark as beginning of a proc
		ATTR_ALIGN = 32  // Mark as align directive
	};

	struct memory_block_t
	{
		uint32       begin;
		uint32       size;
		byte        *memory;
		byte        *attribs;

		memory_block_t(uint32 begin, uint32 size)
			: begin(begin), size(size), memory(0), attribs(0)
		{}
	};

	typedef std::list<memory_block_t> memory_blocks_t;

	memory_blocks_t memory_blocks;
	const memory_block_t *get_block(uint32 ea) const;
	byte *attrib_ref_at(uint32 ea) const;
public:
	void allocate(uint32 begin, uint32 size);

	byte *ref_at(uint32 ea) const;

	void mark_as_code(uint32 ea, uint len = 1);
	void mark_as_flow(uint32 ea);
	void mark_as_proc(uint32 ea);
	void mark_as_align(uint32 ea);

	void unmark_as_proc(uint32 ea);

	bool is_unmarked(uint32 ea, uint len = 1) const;
	bool is_code(uint32 ea) const;
	bool is_op(uint32 ea) const;
	bool is_cont(uint32 ea) const;
	bool is_flow(uint32 ea) const;
	bool is_proc(uint32 ea) const;
	bool is_align(uint32 ea) const;
};

#endif
