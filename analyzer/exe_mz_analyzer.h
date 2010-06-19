#ifndef EXE_MZ_ANALYZER_H
#define EXE_MZ_ANALYZER_H

#include "binaries/exe_mz.h"
#include "support/attributed_memory.h"
#include "support/blocks.h"
#include "support/procs.h"

#include <map>
#include <queue>
#include <set>

struct exe_mz_annotation_t
{
	x86_16_address_t  addr;
	const char       *name;
};

inline
bool operator<(const exe_mz_annotation_t &a, const exe_mz_annotation_t &b)
{
	return a.addr < b.addr;
}

typedef std::vector<exe_mz_annotation_t> exe_mz_annotations_t;

class exe_mz_analyzer_t
{
	typedef std::multimap<x86_16_address_t, x86_16_address_t> edge_map_t;
	typedef std::priority_queue<x86_16_address_t>             addr_queue_t;
	typedef std::set<x86_16_address_t>                        addr_set_t;

	exe_mz_t                  *binary;
	x86_16_segments_t          segments;

	x86_16_seg_t               base_seg;
	attributed_memory_t        memory;

	edge_map_t                 edge;
	edge_map_t                 back_edge;
	addr_set_t                 call_dsts;

	blocks_t                   blocks;
	procs_t                    procs;
	exe_mz_annotations_t       annotations;
public:
	void init(exe_mz_t *abinary);
	void load_annotations(const char *fn);
	void analyze();
	void output(fmt_stream &fs);
	const char *get_annotation_name(x86_16_address_t addr) const;
private:
	void load();
	void relocate();
	void make_segments();
	void trace();
	void analyze_branch(x86_16_address_t addr, const x86_insn &insn, addr_queue_t &cs_ip_queue);
	void analyze_blocks();
	void analyze_procs();

	const char *get_annotation(x86_16_address_t addr) const;
};

#endif
