#include "exe_mz.h"

exe_mz_t::~exe_mz_t()
{
	delete[] image;
}

void exe_mz_t::load(raw_istream_t &is)
{
	head.load(is);

	image_size = 512 * head.e_cp;
	if (head.e_cblp)
		image_size += head.e_cblp - 512;

	image = new byte[image_size];

	is.seek_set(16 * head.e_cparhdr);
	is.read(image, image_size);

	// Read relocations
	relocations.reserve(head.e_crlc);
	is.seek_set(head.e_lfarlc);
	for (int i = 0; i != head.e_crlc; ++i)
	{
		exe_mz_relocation_t reloc;

		reloc.ofs = is.readle16();
		reloc.seg = is.readle16();

		relocations.push_back(reloc);
	}
}

bool exe_mz_header_t::load(raw_istream_t &is)
{
	e_magic     = is.readle16();
	e_cblp      = is.readle16();
	e_cp        = is.readle16();
	e_crlc      = is.readle16();
	e_cparhdr   = is.readle16();
	e_minalloc  = is.readle16();
	e_maxalloc  = is.readle16();
	e_ss        = is.readle16();
	e_sp        = is.readle16();
	e_csum      = is.readle16();
	e_ip        = is.readle16();
	e_cs        = is.readle16();
	e_lfarlc    = is.readle16();
	e_ovno      = is.readle16();

	if (e_lfarlc == 0x40)
	{
		for (int i = 0; i != 4; ++i)
			e_res[i] = is.readle16();
		e_oemid   = is.readle16();
		e_oeminfo = is.readle16();
		for (int i = 0; i != 10; ++i)
			e_res2[i] = is.readle16();
		e_lfanew = is.readle32();
	}
	
	return true;
}

bool exe_mz_header_t::save(raw_ostream_t &os)
{
	os.writele16(e_magic);
	os.writele16(e_cblp);
	os.writele16(e_cp);
	os.writele16(e_crlc);
	os.writele16(e_cparhdr);
	os.writele16(e_minalloc);
	os.writele16(e_maxalloc);
	os.writele16(e_ss);
	os.writele16(e_sp);
	os.writele16(e_csum);
	os.writele16(e_ip);
	os.writele16(e_cs);
	os.writele16(e_lfarlc);
	os.writele16(e_ovno);

	if (e_lfarlc == 0x40)
	{
		for (int i = 0; i != 4; ++i)
			os.writele16(e_res[i]);
		os.writele16(e_oemid);
		os.writele16(e_oeminfo);
		for (int i = 0; i != 10; ++i)
			os.writele16(e_res2[i]);
		os.writele32(e_lfanew);
	}
	
	return true;
}

void exe_mz_header_t::dump()
{
	printf("DOS header\n");
	printf("e_magic     = %04x\n", e_magic);
	printf("e_cblp      = %04x\n", e_cblp);
	printf("e_cp        = %04x\n", e_cp);
	printf("e_crlc      = %04x\n", e_crlc);
	printf("e_cparhdr   = %04x\n", e_cparhdr);
	printf("e_minalloc  = %04x\n", e_minalloc);
	printf("e_maxalloc  = %04x\n", e_maxalloc);
	printf("e_ss        = %04x\n", e_ss);
	printf("e_sp        = %04x\n", e_sp);
	printf("e_csum      = %04x\n", e_csum);
	printf("e_ip        = %04x\n", e_ip);
	printf("e_cs        = %04x\n", e_cs);
	printf("e_lfarlc    = %04x\n", e_lfarlc);
	printf("e_ovno      = %04x\n", e_ovno);

	if (e_lfarlc == 0x40)
	{
		for (int i = 0; i != 4; ++i)
			printf("e_res[%d]    = %04x\n", i, e_res[i]);
		printf("e_oemid     = %04x\n", e_oemid);
		printf("e_oeminfo   = %04x\n", e_oeminfo);
		for (int i = 0; i != 10; ++i)
			printf("e_res2[%d]   = %04x\n", i, e_res2[i]);
		printf("e_lfanew    = %04x\n", e_lfanew);
	}
	putchar('\n');
}
