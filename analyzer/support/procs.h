#ifndef PROCS_H
#define PROCS_H

#include "base/base.h"
#include "x86_analyzer_support.h"

#include <vector>

struct proc_t
{
	x86_16_address_t addr;
	uint32 begin;
	uint32 end;

	const char *name;

	proc_t()
		: begin(0), end(0), name(0)
	{}

	bool contains(uint32 ea) const {
		return begin <= ea && ea < end;
	}
};

inline
bool operator<(const proc_t &a, const proc_t &b)
{
	return a.begin < b.begin;
}

struct procs_t
{
	typedef std::vector<proc_t>            procs_vector_t;
	typedef procs_vector_t::iterator       iterator;
	typedef procs_vector_t::const_iterator const_iterator;

	procs_t()
		: _needs_sorting(false)
	{}

	iterator       begin()       { return procs.begin(); }
	const_iterator begin() const { return procs.begin(); }

	iterator       end()       { return procs.end(); }
	const_iterator end() const { return procs.end(); }

	void sort();

	void push_back(const proc_t &t) { procs.push_back(t); _needs_sorting = true; }

	iterator get_proc(uint32 ea);

private:
	bool _needs_sorting;
	procs_vector_t procs;
};

#endif
