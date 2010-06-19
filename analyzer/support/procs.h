#ifndef PROCS_H
#define PROCS_H

#include "base/base.h"
#include "x86_analyzer_support.h"

#include <vector>

struct proc_t
{
	uint32 begin;
	uint32 end;

	bool contains(uint32 ea) const {
		return begin <= ea && ea < end;
	}
};

struct procs_t
{
	typedef std::vector<proc_t>            procs_vector_t;
	typedef procs_vector_t::iterator       iterator;
	typedef procs_vector_t::const_iterator const_iterator;

	iterator       begin()       { return procs.begin(); }
	const_iterator begin() const { return procs.begin(); }

	iterator       end()       { return procs.end(); }
	const_iterator end() const { return procs.end(); }

	void push_back(const proc_t &t) { procs.push_back(t); }

	iterator get_proc(uint32 ea);

private:
	procs_vector_t procs;
};

#endif
