#ifndef EXE_MZ_H
#define EXE_MZ_H

#include "../base/base.h"
#include "binary.h"

struct exe_mz_loader : public binary_loader {
	std::string name() { return "DOS MZ executable"; }

	void load(raw_istream &is);
};

struct exe_mz_header {      // DOS .EXE header
	uint16 e_magic;                     // Magic number
	uint16 e_cblp;                      // Bytes on last page of file
	uint16 e_cp;                        // Pages in file
	uint16 e_crlc;                      // Relocations
	uint16 e_cparhdr;                   // Size of header in paragraphs
	uint16 e_minalloc;                  // Minimum extra paragraphs needed
	uint16 e_maxalloc;                  // Maximum extra paragraphs needed
	uint16 e_ss;                        // Initial (relative) SS value
	uint16 e_sp;                        // Initial SP value
	uint16 e_csum;                      // Checksum
	uint16 e_ip;                        // Initial IP value
	uint16 e_cs;                        // Initial (relative) CS value
	uint16 e_lfarlc;                    // File address of relocation table
	uint16 e_ovno;                      // Overlay number
	uint16 e_res[4];                    // Reserved words
	uint16 e_oemid;                     // OEM identifier (for e_oeminfo)
	uint16 e_oeminfo;                   // OEM information; e_oemid specific
	uint16 e_res2[10];                  // Reserved words
	uint32 e_lfanew;                    // File address of new exe header

	bool load(raw_istream &is);
	bool save(raw_ostream &os);
	void dump();
};

#endif
