#include "segments.h"

void segments_t::extend_segment_to(segment_t s, uint32 ea)
{
	segments_map_t::iterator i = segments.find(s);

	if (i  == segments.end())
	{
		segments.insert(std::make_pair(s, segment_range_t(ea, ea)));
		return;
	}

	segment_range_t &r = i->second;
	r.begin = std::min(r.begin, ea);
	r.end   = std::max(r.end,   ea);
}

void segments_t::print()
{
	for (segments_map_t::iterator i = segments.begin(); i != segments.end(); ++i)
	{
		segment_t       seg = i->first;
		segment_range_t   r = i->second;

		printf("%04x : [%08lx-%08lx[\n", seg, r.begin, r.end);
	}
}

segofs_t segments_t::addr_to_segofs(uint32 ea)
{
	segofs_t segofs(0, 0);

	for (segments_map_t::iterator i = segments.begin(); i != segments.end(); ++i)
	{
		segment_t       seg = i->first;
		segment_range_t   r = i->second;

		if (ea < r.begin)
			return segofs;

		segofs.seg = seg;
		segofs.ofs = ea - (seg << 4);

		if (r.begin <= ea && ea < r.end)
			return segofs;
	}

	return segofs;
}
