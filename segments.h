#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <map>

#include "ioutils.h"
#include "segofs.h"

#define SEG_CODE  1
#define SEG_DATA  2
#define SEG_STACK 3

struct segments_t {
	typedef uint16 segment_t;

	struct segment_def_t {
		segment_t seg;
		uint32    begin;
		uint32    end;
		byte      type;

		segment_def_t(segment_t seg, uint32 b, uint32 e, byte type)
			: seg(seg), begin(b), end(e), type(type)
		{}
	};

	typedef std::map<segment_t, segment_def_t> segments_map_t;
	segments_map_t segments;

	void extend_segment_to(segment_t s, uint32 ea, byte type);

	void add_segment(segment_t seg);

	segofs_t addr_to_segofs(uint32 ea);

	void print();
};

#endif
