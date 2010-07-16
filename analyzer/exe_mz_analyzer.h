#ifndef EXE_MZ_ANALYZER_H
#define EXE_MZ_ANALYZER_H

#include "binaries/exe_mz.h"
#include "support/annotations.h"
#include "support/attributed_memory.h"
#include "support/blocks.h"
#include "support/edges.h"
#include "support/x86_analyzer_support.h"

#include <map>
#include <queue>
#include <set>

class exe_mz_analyzer_t
{
	typedef std::multimap<uint32, uint32>          edge_map_t;
	typedef std::priority_queue<x86_16_address_t>  addr_queue_t;
	typedef std::set<x86_16_address_t>             addr_set_t;

	exe_mz_t                  *binary;
	x86_16_seg_t               base_seg;
	uint32                     begin_ea;
	uint32                     end_ea;

	attributed_memory_t        memory;
	x86_16_segments_t          segments;

	edges_t                    edges;
	addr_set_t                 call_dsts;

	blocks_t                   blocks;
	annotations_t              annotations;
public:
	void init(exe_mz_t *abinary);
	void load_annotations(const char *fn);
	void analyze();
	void output(fmt_stream &fs);
private:
	void load();
	void relocate();
	void make_segments();
	void trace();
	void analyze_branch(x86_16_address_t addr, const x86_insn &insn, addr_queue_t &cs_ip_queue);
	void analyze_blocks();
	void analyze_procs();

	const char *get_proc_name(uint32 ea) const;
};

#endif
