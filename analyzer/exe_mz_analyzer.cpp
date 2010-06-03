#include "exe_mz_analyzer.h"

#include <queue>

void exe_mz_analyzer_t::init(exe_mz_t *abinary)
{
	binary = abinary;

	// TODO: read annotations
}

void exe_mz_analyzer_t::analyze()
{
	binary->head.dump();
	load();
	make_segments();
	trace();
}

void exe_mz_analyzer_t::load()
{
	base_seg = 0x1000;
	memory.allocate(base_seg, binary->image_size);

	// Load binary into memory
	byte *dst = memory.ref_at(x86_16_address_t(base_seg, 0));
	memcpy(dst, binary->image, binary->image_size);

	relocate();
}

void exe_mz_analyzer_t::relocate()
{
	for (uint i = 0; i != binary->relocations.size(); ++i)
	{
		exe_mz_relocation_t reloc = binary->relocations[i];

		x86_16_address_t addr(base_seg + reloc.seg, reloc.ofs);

		uint16 orig_seg = readle16(memory.ref_at(addr));
		writele16(memory.ref_at(addr), orig_seg + base_seg);
	}
}

void exe_mz_analyzer_t::make_segments()
{
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
	// cs_ip_queue.insert(_annotations.code.begin(); _annotations.code.end());

	while (!cs_ip_queue.empty())
	{
		cs_ip = cs_ip_queue.top();
		cs_ip_queue.pop();

		//printf("%04x:%04x\n", cs_ip.seg, cs_ip.ofs);
		segments.register_address(cs_ip);

		for (;;)
		{
			byte *cs_ip_p = memory.ref_at(cs_ip);

			if (!cs_ip_p) break;
			if (!memory.is_unmarked(cs_ip)) break;

			x86_insn insn = x86_decode(cs_ip_p);

			if (!memory.is_unmarked(cs_ip, insn.op_size)) break;
			memory.mark_as_code(cs_ip, insn.op_size);

			if (x86_16_is_branch(insn))
				analyze_branch(cs_ip, insn, cs_ip_queue);

			cs_ip.ofs += insn.op_size;

			if (x86_16_is_block_stop_op(insn))
				break;
		}
	}
}

void exe_mz_analyzer_t::analyze_branch(x86_16_address_t addr, const x86_insn &insn, exe_mz_analyzer_t::addr_queue_t &cs_ip_queue)
{
	x86_16_address_t dst;

	if (insn.arg[0].kind == KN_ADR)
		dst = x86_16_address_t(insn.arg[0].seg, insn.arg[0].ofs);
	else
	if (insn.arg[0].kind == KN_IMM && insn.arg[0].size == SZ_BYTE)
		dst = x86_16_address_t(addr.seg, addr.ofs + insn.op_size + (int8)insn.arg[0].imm);
	else
	if (insn.arg[0].kind == KN_IMM && insn.arg[0].size == SZ_WORD)
		dst = x86_16_address_t(addr.seg, addr.ofs + insn.op_size + (int16)insn.arg[0].imm);
	else
		return;
	
	cs_ip_queue.push(dst);
}

void exe_mz_analyzer_t::output(fmt_stream &fs) const
{
	x86_16_address_t addr = x86_16_address_t(base_seg, 0);
	uint32 ea = addr.ea();
	uint32 end_ea = ea + binary->image_size;

	bool last_was_data = false;

	while (ea < end_ea)
	{
		addr = segments.addr_at_ea(ea);
		byte *p = memory.ref_at(addr);

		fs.set_line_id("%04x:%04x ", addr.seg, addr.ofs);

		if (memory.is_code(addr))
		{
			x86_insn insn = x86_decode(p);
			char dline[64];
			insn.to_str(dline);
			fs.puts(dline);
			ea += insn.op_size;
		}
		else
		{
			fs.printf("dd  ");
			int i, cnt = 0, o = addr.ofs % 16;
			int rem = 16 - o;

			for (cnt = 0; cnt < rem && ea+cnt < end_ea; ++cnt)
			{
				x86_16_address_t a = segments.addr_at_ea(ea+cnt);
				if (!memory.ref_at(a) || memory.is_code(a))
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
