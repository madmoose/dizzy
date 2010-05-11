#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <queue>

#include "ioutils.h"
#include "x86/x86_disasm.h"

#include "diz.h"
#include "functions.h"
#include "interrupts.h"
#include "names.h"
#include "register_trace.h"
#include "segofs.h"
#include "segments.h"

#define HTML 1

#define UNDEF   0x00
#define OP      0x01
#define CONT    0x02
#define DATA    0x03
#define DEST    0x04

const char *addr_str(segofs_t addr)
{
	static char s[256];
	if (HTML) {
		sprintf(s, "<a name=\"a%08x\">%04x:%04x</a>",
			(addr.seg << 4) + addr.ofs,
			addr.seg, addr.ofs);
	} else {
		sprintf(s, "%04x:%04x",
			addr.seg, addr.ofs);
	}
	return s;
}

const char *addr_link_str(segofs_t addr, const char *text = 0)
{
	static char s[256];
	if (HTML && text) {
		sprintf(s, "<a href=\"#a%08x\">%s</a>",
			(addr.seg << 4) + addr.ofs, text);
	} else if (HTML) {
		sprintf(s, "<a href=\"#a%08x\">%04x:%04x</a>",
			(addr.seg << 4) + addr.ofs,
			addr.seg, addr.ofs);
	} else {
		sprintf(s, "%04x:%04x",
			addr.seg, addr.ofs);
	}
	return s;
}

int read_exe(struct diz_t *diz, char *fn) {
	FILE *f;
	struct exeheader_t head;
	uint32 imagesize;
	int r, i;

	f = fopen(fn, "rb");
	if (!f) return 1;

	head.signature   = freadbe16(f);
	head.extrabytes  = freadle16(f);
	head.pages       = freadle16(f);
	head.relocations = freadle16(f);
	head.headersize  = freadle16(f);
	head.minmemory   = freadle16(f);
	head.maxmemory   = freadle16(f);
	head.initSS      = freadle16(f);
	head.initSP      = freadle16(f);
	head.checksum    = freadle16(f);
	head.initIP      = freadle16(f);
	head.initCS      = freadle16(f);
	head.reloctable  = freadle16(f);
	head.overlay     = freadle16(f);

	printf("head.signature:    %c%c\n", (char)(head.signature >> 8), (char)head.signature);
	printf("extrabytes:     %5d\n",   head.extrabytes);
	printf("pages:          %5d\n",   head.pages);
	printf("relocations:    %5d\n",   head.relocations);
	printf("headersize:     %5d\n",   head.headersize );
	printf("minmemory:      %5d\n",   head.minmemory);
	printf("maxmemory:      %5d\n",   head.maxmemory);
	printf("initSS:          %04x\n", head.initSS);
	printf("initSP:          %04x\n", head.initSP);
	printf("checksum:       %5d\n",   head.checksum);
	printf("initIP:          %04x\n", head.initIP);
	printf("initCS:          %04x\n", head.initCS);
	printf("reloctable:      %04x\n", head.reloctable);
	printf("overlay:        %5d\n",   head.overlay);

	if (head.signature != ('M' << 8 | 'Z') &&
	    head.signature != ('Z' << 8 | 'M'))
		return 1;

	diz->head = head;

	fseek(f, head.reloctable, SEEK_SET);
	for (i = 0; i < head.relocations; i++)
	{
		uint16 seg, ofs;

		ofs = freadle16(f);
		seg = freadle16(f);
		//printf("reloc: %04x:%04x\n", seg, ofs);
	}

	if (head.extrabytes) {
		imagesize = (head.pages-1)*512 - head.headersize*16;
		imagesize += head.extrabytes % 512;
	} else
		imagesize = head.pages*512 - head.headersize*16;

	diz->imagesize = imagesize;

	diz->image     = (byte*)malloc(imagesize);
	assert(diz->image);
	diz->imageattr = (byte*)malloc(imagesize);
	assert(diz->imageattr);
	memset(diz->imageattr, 0, diz->imagesize);

	fseek(f, head.headersize*16, SEEK_SET);

	r = fread(diz->image, imagesize, 1, f);
	assert(r);

	fclose(f);

	return 0;
}

bool is_branch(x86_insn &insn, segofs_t segofs, segofs_t &jsegofs)
{
	switch (insn.op_name)
	{
	case op_call:
	case op_ja:
	case op_jbe:
	case op_jc:
	case op_jcxz:
	case op_je:
	case op_jg:
	case op_jge:
	case op_jl:
	case op_jle:
	case op_jmp:
	case op_jnc:
	case op_jne:
	case op_jno:
	case op_jns:
	case op_jo:
	case op_jpe:
	case op_jpo:
	case op_js:
		break;
	default:
		return false;
	}

	if (insn.arg[0].kind == KN_ADR)
	{
		jsegofs = segofs_t(insn.arg[0].seg, insn.arg[0].ofs);
		return true;
	}

	if (insn.arg[0].kind == KN_IMM && insn.arg[0].size == SZ_BYTE)
	{
		jsegofs = segofs_t(segofs.seg, segofs.ofs + insn.op_size + (int8)insn.arg[0].imm);
		return true;
	}

	if (insn.arg[0].kind == KN_IMM && insn.arg[0].size == SZ_WORD)
	{
		jsegofs = segofs_t(segofs.seg, segofs.ofs + insn.op_size + (int16)insn.arg[0].imm);
		return true;
	}
	
	//char dline[200];
	//insn.to_str(dline);
	//printf("Complex jump or call found at %s: %s\n", addr_link_str(segofs), dline);
	
	return false;
}

bool is_block_stop_op(x86_insn &insn)
{
	return insn.op_name == op_jmp
	    || insn.op_name == op_ret
	    || insn.op_name == op_retf
	    || insn.op_name == op_iret;
}

void diz_t::trace_code()
{
	std::priority_queue<segofs_t> jump_queue(marked_as_code.begin(), marked_as_code.end());
	marked_as_code.clear();

	while (!jump_queue.empty())
	{
		segofs_t segofs = jump_queue.top();
		jump_queue.pop();

		uint32 ea = segofs.addr();

		segments.extend_segment_to(segofs.seg, ea, SEG_CODE);

		bool block_stop;
		do {
			x86_insn insn = x86_decode(image + ea);

			imageattr[ea] = OP;
			for (int i = 1; i < insn.op_size; ++i)
				imageattr[ea + i] = CONT;

			segofs_t jsegofs;
			if (is_branch(insn, segofs, jsegofs))
			{
				uint32 jea = jsegofs.addr();
				if (jea >= imagesize)
				{
					printf("Jump out of image bounds.");
					char dline[200];
					insn.to_str(dline);
					printf("%lx %s\n", ea, dline);
				}
				else if (imageattr[jea] == CONT)
				{
					printf("Jump into instruction.");
					char dline[200];
					insn.to_str(dline);
					printf("%lx %s\n", ea, dline);
				}
				else
				{
					if (imageattr[jea] == UNDEF)
					{
						imageattr[jea] = DEST;
						jump_queue.push(jsegofs);
					}
					edge_map    .insert(std::make_pair(ea, jea));
					edge_map_rev.insert(std::make_pair(jea, ea));

					if (insn.op_name == op_call) {
						functions.add_call_destination(jsegofs);
					}
				}
			}
			segofs += insn.op_size;
			ea = segofs.addr();
			block_stop = is_block_stop_op(insn);
		} while (ea < imagesize && imageattr[ea] == UNDEF && !block_stop);

		segments.extend_segment_to(segofs.seg, ea, SEG_CODE);
	}

	// Mark some bare nop's and retf's as code.
	for (uint s = 0; s < imagesize; ++s)
	{
		if ((image[s] == 0x90 || image[s] == 0xcb) &&
		    imageattr[s] == UNDEF &&
		    (s == 0 || imageattr[s-1] == OP || imageattr[s-1] == CONT)
		   )
		{
			imageattr[s] = OP;
			/*
			uint i;
			for (i = s;
			     i < imagesize &&
			     imageattr[i] == UNDEF &&
			     (image[i] == 0x90 || image[i] == 0xcb);
			     ++i)
			{}

			//if (i == imagesize || imageattr[i] == OP)
			{
				for (uint j = s; j < i; ++j) {
					imageattr[j] = OP;
				}
			}
			*/
		}
	}

	/*
	puts("\nValues assigned to segment register DS:");
	for (std::set<uint16>::const_iterator i = dses.begin(); i != dses.end(); ++i)
		printf("ds: %s%s\n", addr_link_str(segofs_t(*i, 0)), (((uint32)*i << 4) <= imagesize ? "" : " OOB"));
	putchar('\n');
	*/
	/*
	for (std::set<uint16>::const_iterator i = dses.begin(); i != dses.end(); ++i)
		if (*i << 4 <= imagesize)
			segments.add_data_segment(*i);
	*/
}

void diz_t::add_interrupt_comments()
{
	for (uint s = 0; s < imagesize; ++s)
	{
		if (imageattr[s] == OP && image[s] == 0xcd)
		{
			int no = image[s+1];

			if (no != 0x21 && no != 0x10 && no != 0x1A)
				continue;

			int ah = -1;
			for (uint i = s-1; i >= 0 && i > s - 16; --i)
			{
				if (imageattr[i] != OP)
					continue;

				if (image[i] == 0xb4) // mov ah, imm
				{
					ah = image[i+1];
					break;
				}
				else if (image[i] == 0xb8) // mov ax, imm
				{
					// al = image[i + 1];
					ah = image[i + 2];
					break;
				}
			}
			if (ah >= 0)
			{
				segofs_t segofs = segments.addr_to_segofs(s);

				const char *c = 0;

				c = get_dos_int_description(no, ah);

				if (c)
					image_comments[segofs] = c;
			}
		}
	}
}

int main(int argc, char *const argv[])
{
	//read_lib("/Users/madmoose/Games/Old Compilers/test/LIB/LLIBC.LIB");
	//return 0;

	assert(argc == 2);

	diz_t diz;

	if (HTML)
		printf("<!doctype html>\n<html><body>\n<pre>");
	int r = read_exe(&diz, argv[1]);
	assert(r == 0);

	//void mark_zak_op_names(diz_t &diz);
	//mark_zak_op_names(diz);

	for (int i = 0; i != 256; ++i)
		;//printf("0000:%04x\n", readle16(diz.image + 0x0684 + 2 * i));

	diz.mark_as_code(segofs_t(diz.head.initCS, diz.head.initIP));

	names_t *names = load_names("zak.exe.names.txt");
	if (names)
	{
		putchar('\n');
		for (names_t::const_iterator i = names->begin(); i != names->end(); ++i)
		{
			segofs_t addr = i->first;
			diz.mark_as_code(addr);
			printf("%04x:%04x %s\n", addr.seg, addr.ofs, addr_link_str(addr, i->second.c_str()));
		}
	}
	diz.trace_code();
	//diz.add_interrupt_comments();

	puts("\nCode segments");
	diz.segments.print();

	putchar('\n');

	register_trace_t reg_trace;
	reg_trace.clear_all();

	for (uint s = 0; s < diz.imagesize;)
	{
		int bytes_per_line = 16;

		byte op = diz.image[s];
		//if (op == 0x2e) op = diz.image[s+1];

		segofs_t segofs = diz.segments.addr_to_segofs(s);

		std::string *name = get_name(names, segofs);
		if (name)
			printf("\nFunction %s\n\n", name->c_str());

		//printf("imageattr[%x] = %d\n", s, diz.imageattr[s]);
		if (diz.imageattr[s] == UNDEF || diz.imageattr[s] == CONT) {
			int c, i;

			printf("%s (%02x)\t", addr_str(segofs), op);
			printf("dd  ");

			for (i = 0; (bytes_per_line + s - i) % bytes_per_line != 0; ++i)
				printf("   ");

			printf(" %02x", diz.image[s]);
			for (c = 1;
				 s + c < diz.imagesize &&
				 diz.imageattr[s + c] == UNDEF &&
				 (s + c) % bytes_per_line != 0;
				 ++c)
			{
				printf(" %02x", diz.image[s + c]);
			}

			for (i = 0; (c + s + i) % bytes_per_line != 0; ++i)
				printf("   ");

			printf(" | ");

			for (i = 0; (bytes_per_line + s - i) % bytes_per_line != 0; ++i)
				putchar(' ');

			for (i = 0; i < c; ++i) {
				int k = diz.image[s + i];
				printf("%c", isprint(k) ? k : '.');
			}

			for (i = 0; (c + s + i) % bytes_per_line != 0; ++i)
				putchar(' ');

			putchar('|');
			putchar('\n');

			s += c;
		} else if (diz.imageattr[s] == OP) {
			std::pair<edge_map_t::const_iterator, edge_map_t::const_iterator> r;

			r = diz.edge_map_rev.equal_range(s);
			if (r.first != r.second)
			{
				reg_trace.clear_all();
				printf("<-");
				for (edge_map_t::const_iterator i = r.first;
				     i != r.second;
				     ++i)
				{
					segofs_t jsegofs = diz.segments.addr_to_segofs(i->second);
					printf(" %s", addr_link_str(jsegofs));
				}
				putchar('\n');
			}

			printf("%s (%02x)\t", addr_str(segofs), op);
			x86_insn insn = x86_decode(diz.image + s);
			int op_size = insn.op_size;

			reg_trace.apply_insn(insn);

			for (int i = 0; i != op_size; ++i)
				printf("%02x", diz.image[s+i]);
			for (int i = op_size; i < 8; ++i)
				printf("  ");
			printf(" ");

			char dline[200];
			if (insn.op_name == op_call)
			{
				segofs_t dst;
				is_branch(insn, segofs, dst);
				std::string *dst_name = get_name(names, dst);
				if (dst_name)
					sprintf(dline, "call %s", addr_link_str(dst, dst_name->c_str()));
				else
					insn.to_str(dline);
			}
			else
			{
				insn.to_str(dline);
			}
			printf("%s", dline);

			dline[0] = '\0';
			reg_trace.dump(dline);
			printf(" \t%s", dline);

			//dline[0] = '\0';
			//insn.dump(dline);
			//printf(" \t[%s]", dline);

			comment_map_t::const_iterator comment_i = diz.image_comments.find(segofs);
			if (comment_i != diz.image_comments.end())
			{
				printf("              %s", comment_i->second);
			}

			r = diz.edge_map.equal_range(s);
			if (r.first != r.second)
			{
				printf("\t->");
				for (edge_map_t::const_iterator i = r.first; i != r.second; ++i)
				{
					segofs_t jsegofs = diz.segments.addr_to_segofs(i->second);
					printf(" %s", addr_link_str(jsegofs));
				}
			}

			putchar('\n');
			s += op_size;

		} else if (diz.imageattr[s] == DATA) {
			printf("%s (%02x)\t", addr_str(segofs), op);
			printf("dw  %04x", readle16(&diz.image[s]));
			s += 2;

			std::pair<edge_map_t::const_iterator, edge_map_t::const_iterator> r;
			r = diz.edge_map.equal_range(s);
			if (r.first != r.second)
			{
				printf("\t->");
				for (edge_map_t::const_iterator i = r.first;
				     i != r.second;
				     ++i)
				{
					segofs_t jsegofs = diz.segments.addr_to_segofs(i->second);
					printf(" %s", addr_link_str(jsegofs));
				}
			}

			putchar('\n');
			//assert(0);
		} else
		{
			printf("(%d)", diz.imageattr[s]);
		}
	}

	if (HTML)
		puts("</pre>\n</body></html>");
	free(diz.image);
	free(diz.imageattr);
}

