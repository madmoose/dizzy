#include "exe_mz_analyzer.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>

#include "support/annotations.h"
#include "support/blocks.h"
#include "support/procs.h"

void exe_mz_analyzer_t::init(exe_mz_t *abinary)
{
	binary = abinary;
}

void exe_mz_analyzer_t::load_annotations(const char *fn)
{
	puts("\nAnnotations:");
	std::ifstream ifs(fn);
	std::string line;
	while (std::getline(ifs, line))
	{
		std::istringstream ss(line);

		char t, c;
		uint16 seg, ofs;
		std::string name;

		ss >> t >> std::hex >> seg >> c >> ofs >> name;
		printf("%c %04x:%04x %s\n", t, seg, ofs, name.c_str());

		proc_t proc;
		proc.name = strdup(name.c_str());
		proc.addr = x86_16_address_t(seg, ofs);
		proc.begin(proc.addr.ea());
		proc.end(proc.addr.ea());

		annotations.procs->insert(proc);
	}
	putchar('\n');
}

void exe_mz_analyzer_t::analyze()
{
	binary->head.dump();
	load();
	make_segments();
	trace();
	analyze_blocks();
	analyze_procs();
}

void exe_mz_analyzer_t::load()
{
	base_seg = 0x1000;
	memory.allocate(base_seg << 4, binary->image_size);

	// Load binary into memory
	byte *dst = memory.ref_at(base_seg << 4);
	memcpy(dst, binary->image, binary->image_size);

	relocate();
}

void exe_mz_analyzer_t::relocate()
{
	for (uint i = 0; i != binary->relocations.size(); ++i)
	{
		exe_mz_relocation_t reloc = binary->relocations[i];

		uint32 ea = x86_16_address_t(base_seg + reloc.seg, reloc.ofs).ea();
		byte *p = memory.ref_at(ea);

		uint16 orig_seg = readle16(p);
		writele16(p, orig_seg + base_seg);
	}
}

void exe_mz_analyzer_t::make_segments()
{
	// Make initial cs segment
	segments.make_segment(base_seg + binary->head.e_cs);

	// Make segments from relocations
	for (uint i = 0; i != binary->relocations.size(); ++i)
	{
		exe_mz_relocation_t reloc = binary->relocations[i];

		x86_16_address_t addr(reloc.seg, reloc.ofs);
		byte *p = binary->image + addr.ea();

		uint16 seg = readle16(p);

		segments.make_segment(base_seg + seg);
	}

	// Make stack segment
	segments.make_segment(base_seg + binary->head.e_ss);
}

void exe_mz_analyzer_t::trace()
{
	x86_16_address_t                      init_cs_ip;
	x86_16_address_t                      cs_ip;
	std::priority_queue<x86_16_address_t> cs_ip_queue;

	init_cs_ip = x86_16_address_t(base_seg + binary->head.e_cs, binary->head.e_ip);
	cs_ip_queue.push(init_cs_ip);

	// Add proc entry points
	for (procs_t::const_iterator i = annotations.procs->begin(),
	                             e = annotations.procs->end(); i != e; ++i)
	{
		cs_ip_queue.push(i->addr);
		memory.mark_as_proc(i->begin());
	}

	while (!cs_ip_queue.empty())
	{
		cs_ip = cs_ip_queue.top();
		cs_ip_queue.pop();

		//printf("%04x:%04x\n", cs_ip.seg, cs_ip.ofs);
		bool is_continuation = false;

		for (;;)
		{
			uint32 cs_ip_ea = cs_ip.ea();
			byte *cs_ip_p = memory.ref_at(cs_ip_ea);

			if (!cs_ip_p) break;

			if (is_continuation)
				memory.mark_as_flow(cs_ip_ea);
			if (memory.is_code(cs_ip_ea)) break;

			x86_insn insn = x86_decode(cs_ip_p);

			//if (!memory.is_unmarked(cs_ip_ea, insn.op_size)) break;
			memory.mark_as_code(cs_ip_ea, insn.op_size);
			segments.register_address(cs_ip);
			segments.register_address(x86_16_address_t(cs_ip.seg, cs_ip.ofs + insn.op_size - 1));

			is_continuation = true;

			if (x86_16_is_branch(insn))
				analyze_branch(cs_ip, insn, cs_ip_queue);

			byte *p = cs_ip_p;
			// TODO: Remove gross hack :)
			bool block_stop = (p[0] == 0xcd && p[1] == 0x21 &&
			                   p[-3] == 0xb8 && p[-2] == 0x01 && p[-1] == 0x4c);
			block_stop = block_stop || x86_16_is_block_stop_op(insn);

			if (block_stop)
				break;

			cs_ip.ofs += insn.op_size;
		}
	}

	uint32 base_ea = base_seg << 4;
	for (uint32 ea = base_ea; ea < base_ea + binary->image_size; ++ea)
	{
		if ((*memory.ref_at(ea) == 0x90 || *memory.ref_at(ea) == 0xcb) &&
		    memory.is_unmarked(ea) &&
		    (ea == base_ea || memory.is_code(ea-1) || memory.is_align(ea-1) || memory.is_code(ea-1))
		   )
		{
			memory.mark_as_align(ea);
		}
	}
}

void exe_mz_analyzer_t::analyze_blocks()
{
	uint32 begin_ea = base_seg << 4;
	uint32 end_ea;
	uint32 image_end = begin_ea + binary->image_size;

	for (;;)
	{
		// Find beginning of block
		while (begin_ea != image_end && !memory.is_op(begin_ea))
			++begin_ea;
		if (begin_ea == image_end)
			break;
		assert(!memory.is_flow(begin_ea));

		// Find end of block
		end_ea = begin_ea + 1;
		while (end_ea != image_end && ((memory.is_op(end_ea) && memory.is_flow(end_ea)) || memory.is_cont(end_ea)))
			++end_ea;

		// Back up and find last op before end_ea
		uint32 last_op = end_ea;
		while (!memory.is_op(--last_op))
			;
		x86_insn insn = x86_decode(memory.ref_at(last_op));

		// Record block information
		block_t block;
		block.begin(begin_ea);
		block.end(end_ea);
		block.terminator_op_name = insn.op_name;

		blocks.insert(block);

		if (end_ea == image_end)
			break;
		begin_ea = end_ea;
	}
}

void exe_mz_analyzer_t::analyze_procs()
{
	// Add a proc for the initial cs:ip
	x86_16_address_t init_cs_ip = x86_16_address_t(base_seg + binary->head.e_cs, binary->head.e_ip);
	if (!memory.is_proc(init_cs_ip.ea()))
	{
		memory.mark_as_proc(init_cs_ip.ea());

		proc_t proc;
		proc.addr = init_cs_ip;
		proc.begin(init_cs_ip.ea());

		char *name;
		asprintf(&name, "_start");
		proc.name = name;

		annotations.procs->insert(proc);
	}

	// Go through all the registered call destinations and create procs
	// starting there
	for (addr_set_t::iterator i = call_dsts.begin(),
	                          e = call_dsts.end(); i != e; ++i)
	{
		if (memory.is_proc(i->ea()))
			continue;
		memory.mark_as_proc(i->ea());

		proc_t proc;
		proc.addr = *i;
		proc.begin(i->ea());

		char *name;
		asprintf(&name, "sub_%x", proc.begin());
		proc.name = name;

		annotations.procs->insert(proc);
	}

	for (procs_t::iterator pi = annotations.procs->begin(),
	                       pe = annotations.procs->end();
	                       pi != pe; ++pi)
	{
		procs_t::iterator next_pi = pi;
		++next_pi;
		uint32 next_proc_ea =
			next_pi != annotations.procs->end()
			? next_pi->begin()
			: ((base_seg << 4) + binary->image_size);

		blocks_t::iterator bi = blocks.find(pi->begin());
		assert(bi != blocks.end());

		while (bi != blocks.end() &&
		       bi->begin() < next_proc_ea
		       && bi->terminator_op_name != op_ret)
		{
			++bi;
		}

		if (bi == blocks.end() || bi->end() > next_proc_ea)
			--bi;

		pi->end(bi->end());

		if (pi->end() <= pi->begin())
		{
			printf("Function annotation %04x:%04x '%s' invalid. (%x-%x)\n", pi->addr.seg, pi->addr.ofs, pi->name, pi->begin(), pi->end());
			memory.unmark_as_proc(pi->begin());
		}
	}

	/*
	for (procs_t::const_iterator pi = annotations.procs->begin(); pi != annotations.procs->end(); ++pi)
		printf("%04x:%04x  %6x - %6x  %s\n", pi->addr.seg, pi->addr.ofs, pi->begin(), pi->end(), pi->name);
	*/
}

void exe_mz_analyzer_t::analyze_branch(x86_16_address_t addr, const x86_insn &insn, exe_mz_analyzer_t::addr_queue_t &cs_ip_queue)
{
	x86_16_address_t dst;

	if (!x86_16_branch_destination(insn, addr, &dst))
		return;

	edge.insert(std::make_pair(addr, dst));
	back_edge.insert(std::make_pair(dst, addr));

	if (insn.op_name == op_call)
		call_dsts.insert(dst);

	cs_ip_queue.push(dst);
}

void exe_mz_analyzer_t::output(fmt_stream &fs)
{
	x86_16_address_t addr = x86_16_address_t(base_seg, 0);
	uint32 ea = addr.ea();
	uint32 end_ea = ea + binary->image_size;

	x86_16_address_t cur_proc_addr;
	procs_t::iterator cur_proc_i = annotations.procs->end();

	segments.dump();

	while (ea < end_ea)
	{
		byte *p = memory.ref_at(ea);

		addr = segments.addr_at_ea(ea);
		fs.set_line_id("%04x:%04x ", addr.seg, addr.ofs);

		if (memory.is_code(ea))
		{
			if (memory.is_proc(ea))
			{
				fs.printf("\n%s ", get_proc_name(ea));

				if (fs.col() < 27)
					fs.set_col(27);
				fs.puts("proc");

				cur_proc_i = annotations.procs->find(ea);
				cur_proc_addr = addr;
			}
			else if (!memory.is_flow(ea))
				fs.printf("; ---------------------------------------------------------------------------\n\n");

			x86_insn insn = x86_decode(p);

			fs.set_col(27);

			char dline[64];
			insn.to_str(dline, addr, &annotations);
			fs.puts(dline);

			ea += insn.op_size;

			if (cur_proc_i != annotations.procs->end() && cur_proc_i->end() == ea)
			{
				fs.printf("%s ", get_proc_name(cur_proc_addr.ea()));

				if (fs.col() < 27)
					fs.set_col(27);
				fs.puts("endp");

				cur_proc_i = annotations.procs->end();
			}
		}
		else if (memory.is_align(ea))
		{
			fs.puts("");
			fs.set_col(27);
			fs.printf("align\n");
			++ea;
		}
		else
		{
			fs.printf("dd  ");
			int i, cnt = 0, o = addr.ofs % 16;
			int rem = 16 - o;

			for (cnt = 0; cnt < rem && ea+cnt < end_ea; ++cnt)
			{
				if (!memory.ref_at(ea+cnt) || memory.is_code(ea+cnt))
					break;
			}

			for (i = 0; i != o; ++i)
				fs.printf("   ");

			for (i = 0; i < cnt; ++i)
			{
				if (i)
					fs.putchar(' ');
				fs.printf("%02x", p[i]);
			}
			for (i = cnt; i < rem; ++i)
				fs.printf("   ");

			fs.printf(" |");
			for (i = 0; i != o; ++i)
				fs.printf(" ");
			for (i = 0; i < cnt; ++i)
				fs.printf("%c", isprint(p[i]) ? p[i] : '.');
			for (i = cnt; i < rem; ++i)
				fs.putchar(' ');
			fs.printf("|");

			fs.putchar('\n');
			ea += cnt;
		}
	}
}

const char *exe_mz_analyzer_t::get_proc_name(uint32 ea) const
{
	assert(memory.is_proc(ea));

	procs_t::const_iterator pi = annotations.procs->find(ea);
	assert(pi != annotations.procs->end());

	return pi->name;
}
