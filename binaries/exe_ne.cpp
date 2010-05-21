#include "exe_ne.h"
#include "exe_mz.h"

void exe_ne_loader_t::load(raw_istream_t &is, uint32 base)
{
	exe_mz_header_t mz_head;
	mz_head.load(is);

	is.seek_set(mz_head.e_lfanew);

	exe_ne_header_t ne_head;
	ne_head.load(is);

	ne_head.dump();

	// Segment table
	is.seek_set(mz_head.e_lfanew + ne_head.ne_segtab);
	for (int i = 0; i != ne_head.ne_cseg; ++i)
	{
		uint16 ofs = is.readle16();
		uint16 len = is.readle16();
		uint16 flg = is.readle16();
		uint16 alo = is.readle16();

		printf("%04x %04x %04x %04x\n", ofs, len, flg, alo);
	}
}

bool exe_ne_header_t::load(raw_istream_t &is)
{
	ne_magic        = is.readle16();
	ne_ver          = is.readbyte();
	ne_rev          = is.readbyte();
	ne_enttab       = is.readle16();
	ne_cbenttab     = is.readle16();
	ne_crc          = is.readle32();
	ne_flags        = is.readle16();
	ne_autodata     = is.readle16();
	ne_heap         = is.readle16();
	ne_stack        = is.readle16();
	ne_csip         = is.readle32();
	ne_sssp         = is.readle32();
	ne_cseg         = is.readle16();
	ne_cmod         = is.readle16();
	ne_cbnrestab    = is.readle16();
	ne_segtab       = is.readle16();
	ne_rsrctab      = is.readle16();
	ne_restab       = is.readle16();
	ne_modtab       = is.readle16();
	ne_imptab       = is.readle16();
	ne_nrestab      = is.readle32();
	ne_cmovent      = is.readle16();
	ne_align        = is.readle16();
	ne_cres         = is.readle16();
	ne_exetyp       = is.readbyte();
	ne_flagsothers  = is.readbyte();
	ne_pretthunks   = is.readle16();
	ne_psegrefbytes = is.readle16();
	ne_swaparea     = is.readle16();
	ne_expver       = is.readle16();
	
	return true;
}

bool exe_ne_header_t::save(raw_ostream_t &os)
{
	os.writele16(ne_magic);
	os.writebyte(ne_ver);
	os.writebyte(ne_rev);
	os.writele16(ne_enttab);
	os.writele16(ne_cbenttab);
	os.writele32(ne_crc);
	os.writele16(ne_flags);
	os.writele16(ne_autodata);
	os.writele16(ne_heap);
	os.writele16(ne_stack);
	os.writele32(ne_csip);
	os.writele32(ne_sssp);
	os.writele16(ne_cseg);
	os.writele16(ne_cmod);
	os.writele16(ne_cbnrestab);
	os.writele16(ne_segtab);
	os.writele16(ne_rsrctab);
	os.writele16(ne_restab);
	os.writele16(ne_modtab);
	os.writele16(ne_imptab);
	os.writele32(ne_nrestab);
	os.writele16(ne_cmovent);
	os.writele16(ne_align);
	os.writele16(ne_cres);
	os.writebyte(ne_exetyp);
	os.writebyte(ne_flagsothers);
	os.writele16(ne_pretthunks);
	os.writele16(ne_psegrefbytes);
	os.writele16(ne_swaparea);
	os.writele16(ne_expver);
	
	return true;
}

void exe_ne_header_t::dump()
{
	printf("NE header\n");
	printf("ne_magic        =     %04x\n", ne_magic);
	printf("ne_ver          =       %02x\n", ne_ver);
	printf("ne_rev          =       %02x\n", ne_rev);
	printf("ne_enttab       =     %04x\n", ne_enttab);
	printf("ne_cbenttab     =     %04x\n", ne_cbenttab);
	printf("ne_crc          = %08x\n", ne_crc);
	printf("ne_flags        =     %04x\n", ne_flags);
	printf("ne_autodata     =     %04x\n", ne_autodata);
	printf("ne_heap         =     %04x\n", ne_heap);
	printf("ne_stack        =     %04x\n", ne_stack);
	printf("ne_csip         = %08x\n", ne_csip);
	printf("ne_sssp         = %08x\n", ne_sssp);
	printf("ne_cseg         =     %04x\n", ne_cseg);
	printf("ne_cmod         =     %04x\n", ne_cmod);
	printf("ne_cbnrestab    =     %04x\n", ne_cbnrestab);
	printf("ne_segtab       =     %04x\n", ne_segtab);
	printf("ne_rsrctab      =     %04x\n", ne_rsrctab);
	printf("ne_restab       =     %04x\n", ne_restab);
	printf("ne_modtab       =     %04x\n", ne_modtab);
	printf("ne_imptab       =     %04x\n", ne_imptab);
	printf("ne_nrestab      = %08x\n", ne_nrestab);
	printf("ne_cmovent      =     %04x\n", ne_cmovent);
	printf("ne_align        =     %04x\n", ne_align);
	printf("ne_cres         =     %04x\n", ne_cres);
	printf("ne_exetyp       =       %02x\n", ne_exetyp);
	printf("ne_flagsothers  =       %02x\n", ne_flagsothers);
	printf("ne_pretthunks   =     %04x\n", ne_pretthunks);
	printf("ne_psegrefbytes =     %04x\n", ne_psegrefbytes);
	printf("ne_swaparea     =     %04x\n", ne_swaparea);
	printf("ne_expver       =     %04x\n", ne_expver);
	putchar('\n');
}
