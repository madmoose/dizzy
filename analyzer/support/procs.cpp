#include "procs.h"

void procs_t::sort()
{
	if (_needs_sorting)
		std::sort(procs.begin(), procs.end());
	_needs_sorting = false;
}

procs_t::iterator procs_t::get_proc(uint32 ea)
{
	sort();
	procs_t::iterator b = procs.begin();
	procs_t::iterator e = procs.end();

	// Find lower bound
	while (e - b > 0)
	{
		procs_t::iterator mid = b + (e - b) / 2;
		if (mid->begin < ea)
			b = mid + 1;
		else
			e = mid;
	}

	if (b == procs.end() || (ea < b->begin && b != procs.begin()))
		--b;


	if (b->contains(ea))
		return b;

	return procs.end();
}
