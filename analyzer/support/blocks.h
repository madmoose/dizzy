#ifndef BLOCKS_H
#define BLOCKS_H

#include "base/base.h"
#include "x86_analyzer_support.h"
#include "x86/x86_disasm.h"

#include <vector>

struct block_t
{
	uint32 begin;
	uint32 end;
	e_ops  terminator_op_name;

	bool contains(uint32 ea) const {
		return begin <= ea && ea < end;
	}
};

struct blocks_t
{
	typedef std::vector<block_t>            blocks_vector_t;
	typedef blocks_vector_t::iterator       iterator;
	typedef blocks_vector_t::const_iterator const_iterator;

	iterator       begin()       { return blocks.begin(); }
	const_iterator begin() const { return blocks.begin(); }

	iterator       end()       { return blocks.end(); }
	const_iterator end() const { return blocks.end(); }

	void push_back(const block_t &t) { blocks.push_back(t); }

	iterator get_block(uint32 ea);

private:
	blocks_vector_t blocks;
};

#endif
