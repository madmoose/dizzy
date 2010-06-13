#include "exe_mz_analyzer.h"

#include <queue>

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

			//if (!memory.is_unmarked(cs_ip, insn.op_size)) break;
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

void exe_mz_analyzer_t::analyze_procs()
{
	for (addr_set_t::const_iterator i = call_dsts.begin(); i != call_dsts.end(); ++i)
		memory.mark_as_proc(i->ea());
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

void exe_mz_analyzer_t::output(fmt_stream &fs) const
{
	x86_16_address_t addr = x86_16_address_t(base_seg, 0);
	uint32 ea = addr.ea();
	uint32 end_ea = ea + binary->image_size;

	while (ea < end_ea)
	{
		byte *p = memory.ref_at(ea);

		addr = segments.addr_at_ea(ea);
		fs.set_line_id("%04x:%04x ", addr.seg, addr.ofs);

		exe_mz_annotation_t key;
		key.addr = addr;
		std::pair<exe_mz_annotations_t::const_iterator, exe_mz_annotations_t::const_iterator> result =
			std::equal_range(annotations.begin(), annotations.end(), key);

		if (memory.is_code(ea))
		{
			if (memory.is_proc(ea))
			{
				if (result.first != result.second)
					fs.printf("\n%s", result.first->name);
				else
					fs.printf("\nsub_%x", ea);
				fs.set_col(27);
				fs.puts("proc");
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
