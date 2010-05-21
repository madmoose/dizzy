#ifndef EXE_NE_H
#define EXE_NE_H

#include "../base/base.h"
#include "binary.h"

struct exe_ne_loader_t : public binary_loader_t {
	std::string name() { return "Windows NE executable"; }

        void load(raw_istream_t &is, uint32 base);
};

struct exe_ne_header_t {      // NE .EXE header
	uint16 ne_magic;                    // Magic number
	byte   ne_ver;                      // Version number
	byte   ne_rev;                      // Revision number
	uint16 ne_enttab;                   // Offset of Entry Table
	uint16 ne_cbenttab;                 // Number of bytes in Entry Table
	uint32 ne_crc;                      // Checksum of whole file
	uint16 ne_flags;                    // Flag UINT16
	uint16 ne_autodata;                 // Automatic data segment number
	uint16 ne_heap;                     // Initial heap allocation
	uint16 ne_stack;                    // Initial stack allocation
	uint32 ne_csip;                     // Initial CS:IP setting
	uint32 ne_sssp;                     // Initial SS:SP setting
	uint16 ne_cseg;                     // Count of file segments
	uint16 ne_cmod;                     // Entries in Module Reference Table
	uint16 ne_cbnrestab;                // Size of non-resident name table
	uint16 ne_segtab;                   // Offset of Segment Table
	uint16 ne_rsrctab;                  // Offset of Resource Table
	uint16 ne_restab;                   // Offset of resident name table
	uint16 ne_modtab;                   // Offset of Module Reference Table
	uint16 ne_imptab;                   // Offset of Imported Names Table
	uint32 ne_nrestab;                  // Offset of Non-resident Names Table
	uint16 ne_cmovent;                  // Count of movable entries
	uint16 ne_align;                    // Segment alignment shift count
	uint16 ne_cres;                     // Count of resource segments
	byte   ne_exetyp;                   // Target Operating system
	byte   ne_flagsothers;              // Other .EXE flags
	uint16 ne_pretthunks;               // offset to return thunks
	uint16 ne_psegrefbytes;             // offset to segment ref. bytes
	uint16 ne_swaparea;                 // Minimum code swap area size
	uint16 ne_expver;                   // Expected Windows version number

	bool load(raw_istream_t &is);
	bool save(raw_ostream_t &os);
	void dump();
};

#endif
