#ifndef EXE_MZ_ANALYZER_H
#define EXE_MZ_ANALYZER_H

#include "../binaries/exe_mz.h"
#include "x86_analyzer_support.h"

#include <queue>

class exe_mz_analyzer_t
{
	exe_mz_t                  *binary;
	x86_16_segments_t          segments;

	x86_16_seg_t               base_seg;
	x86_16_attributed_memory_t memory;

	typedef std::priority_queue<x86_16_address_t> addr_queue_t;
public:
	void init(exe_mz_t *abinary);
	void analyze();
	void dump();

	void output(fmt_stream &fs) const;
private:
	void load();
	void relocate();
	void make_segments();
	void trace();
	void analyze_branch(x86_16_address_t addr, const x86_insn &insn, addr_queue_t &cs_ip_queue);

};

#endif
