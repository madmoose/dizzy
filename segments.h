#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <map>

#include "ioutils.h"
#include "segofs.h"

struct segments_t {
	typedef uint16 segment_t;
	struct segment_range_t {
		uint32 begin;
		uint32 end;

		segment_range_t(uint32 b, uint32 e)
			: begin(b), end(e)
		{}
	};

	typedef std::map<segment_t, segment_range_t> segments_map_t;
	segments_map_t segments;

	void extend_segment_to(segment_t s, uint32 ea);

	void add_segment(segment_t seg);

	segofs_t addr_to_segofs(uint32 ea);

	void print();
};

#endif
