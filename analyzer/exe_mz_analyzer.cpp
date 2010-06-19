#include "exe_mz_analyzer.h"

#include <queue>

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

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

		exe_mz_annotation_t a;
		a.addr = x86_16_address_t(seg, ofs);
		a.name = strdup(name.c_str());

		annotations.push_back(a);
	}
	putchar('\n');

	std::sort(annotations.begin(), annotations.end());
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

	segments.dump();
}

void exe_mz_analyzer_t::trace()
{
	x86_16_address_t                      init_cs_ip;
	x86_16_address_t                      cs_ip;
	std::priority_queue<x86_16_address_t> cs_ip_queue;

	init_cs_ip = x86_16_address_t(base_seg + binary->head.e_cs, binary->head.e_ip);
	cs_ip_queue.push(init_cs_ip);

	// Add annotations
	for (exe_mz_annotations_t::const_iterator i = annotations.begin(); i != annotations.end(); ++i)
	{
		cs_ip_queue.push(i->addr);
		memory.mark_as_proc(i->addr.ea());
	}

	while (!cs_ip_queue.empty())
	{
		cs_ip = cs_ip_queue.top();
		cs_ip_queue.pop();

		//printf("%04x:%04x\n", cs_ip.seg, cs_ip.ofs);
		segments.register_address(cs_ip);

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
		block.begin = begin_ea;
		block.end   = end_ea;
		block.terminator_op_name = insn.op_name;

		blocks.push_back(block);

		if (end_ea == image_end)
			break;
		begin_ea = end_ea;
	}
}

void exe_mz_analyzer_t::analyze_procs()
{
	for (addr_set_t::const_iterator i = call_dsts.begin(); i != call_dsts.end(); ++i)
	{
		memory.mark_as_proc(i->ea());

		proc_t proc;
		proc.begin = i->ea();
		procs.push_back(proc);
	}

	for (procs_t::iterator pi = procs.begin(); pi != procs.end(); ++pi)
	{
		procs_t::iterator next_pi = pi + 1;
		uint32 next_proc_ea = next_pi != procs.end() ? next_pi->begin : ((base_seg << 4) + binary->image_size);

		blocks_t::iterator bi = blocks.get_block(pi->begin);
		while (bi != blocks.end() && bi->begin < next_proc_ea && bi->terminator_op_name != op_ret)
			++bi;

		if (next_proc_ea <= bi->begin)
			--bi;

		if (bi != blocks.end())
			pi->end = bi->end;
	}
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
	procs_t::iterator cur_proc_i = procs.end();

	while (ea < end_ea)
	{
		byte *p = memory.ref_at(ea);

		addr = segments.addr_at_ea(ea);
		fs.set_line_id("%04x:%04x ", addr.seg, addr.ofs);

		if (memory.is_code(ea))
		{
			if (memory.is_proc(ea))
			{
				const char *name = get_annotation_name(addr);
				if (name)
					fs.printf("\n%s", name);
				else
					fs.printf("\nsub_%x", ea);
				fs.set_col(27);
				fs.puts("proc");

				cur_proc_i = procs.get_proc(ea);
				cur_proc_addr = addr;
			}
			else if (!memory.is_flow(ea))
				fs.printf("; ---------------------------------------------------------------------------\n\n");

			x86_insn insn = x86_decode(p);

			fs.set_col(27);

			x86_16_address_t dst;
			const char *name;
			if (insn.op_name == op_call &&
			    x86_16_branch_destination(insn, addr, &dst) &&
			    (name = get_annotation_name(dst)))
			{
				fs.printf("call %s\n", name);
			}
			else
			{
				char dline[64];
				insn.to_str(dline);
				fs.puts(dline);
			}

			ea += insn.op_size;

			if (cur_proc_i != procs.end() && cur_proc_i->end == ea)
			{
				const char *name = get_annotation_name(cur_proc_addr);
				if (name)
					fs.printf("%s", name);
				else
					fs.printf("sub_%x", cur_proc_i->begin);
				fs.set_col(27);
				fs.puts("endp");

				cur_proc_i = procs.end();
			}
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

const char *exe_mz_analyzer_t::get_annotation_name(x86_16_address_t addr) const
{
		exe_mz_annotation_t key;
		key.addr = addr;
		std::pair<exe_mz_annotations_t::const_iterator, exe_mz_annotations_t::const_iterator> result =
			std::equal_range(annotations.begin(), annotations.end(), key);

		if (result.first == result.second)
			return 0;

		return result.first->name;
}
