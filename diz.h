#ifndef DIZ_H
#define DIZ_H

#include "ioutils.h"

#include "functions.h"
#include "segofs.h"
#include "segments.h"

struct exeheader_t {
	uint16 signature;       /* EXE Signature MZ or ZM */
	uint16 extrabytes;      /* Bytes on the last page */
	uint16 pages;           /* Pages in file */
	uint16 relocations;     /* Relocations in file */
	uint16 headersize;      /* Paragraphs in header */
	uint16 minmemory;       /* Minimum amount of memory */
	uint16 maxmemory;       /* Maximum amount of memory */
	uint16 initSS;
	uint16 initSP;
	uint16 checksum;
	uint16 initIP;
	uint16 initCS;
	uint16 reloctable;
	uint16 overlay;
};

typedef std::multimap<uint32, uint32> edge_map_t;
typedef std::map<segofs_t, const char *> comment_map_t;

typedef edge_map_t::const_iterator edge_ref_t;

struct diz_t {
	struct exeheader_t head;
	uint32      imagesize;
	byte       *image;
	byte       *imageattr;

	edge_map_t edge_map;
	edge_map_t edge_map_rev;

	comment_map_t image_comments;

	segments_t  segments;
	functions_t functions;

	std::set<segofs_t> marked_as_code;
	void mark_as_code(segofs_t s) {
		marked_as_code.insert(s);
	}

	void trace_code();
	void add_interrupt_comments();
};

#endif
