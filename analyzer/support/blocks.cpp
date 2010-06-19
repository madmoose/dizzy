#include "blocks.h"

blocks_t::iterator blocks_t::get_block(uint32 ea)
{
	blocks_t::iterator b = blocks.begin();
	blocks_t::iterator e = blocks.end();

	// Find lower bound
	while (e - b > 0)
	{
		blocks_t::iterator mid = b + (e - b) / 2;
		if (mid->begin < ea)
			b = mid + 1;
		else
			e = mid;
	}
	if (ea < b->begin && b != blocks.begin())
		--b;

	if (b->contains(ea))
		return b;

	return blocks.end();
}
