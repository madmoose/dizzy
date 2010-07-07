#include "x86_disasm.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyzer/support/procs.h"
#include "analyzer/support/annotations.h"

const char *str_ops[] = {
	"<illegal>",
	"aaa",
	"aad",
	"aam",
	"aas",
	"adc",
	"add",
	"and",
	"call",
	"cbw",
	"clc",
	"cld",
	"cli",
	"cmc",
	"cmp",
	"cmpsb",
	"cmpsw",
	"cwd",
	"daa",
	"das",
	"dec",
	"div",
	"grp1",
	"grp2",
	"grp3a",
	"grp3b",
	"grp4",
	"grp5",
	"grp6",
	"grp7",
	"grp8",
	"hlt",
	"idiv",
	"imul",
	"in",
	"inc",
	"int",
	"into",
	"iret",
	"ja",
	"jae",
	"jb",
	"jbe",
	"jc",
	"jcxz",
	"je",
	"jg",
	"jge",
	"jl",
	"jle",
	"jmp",
	"jna",
	"jnae",
	"jnb",
	"jnbe",
	"jnc",
	"jne",
	"jnge",
	"jnl",
	"jnle",
	"jno",
	"jnoe",
	"jnp",
	"jns",
	"jnz",
	"jo",
	"jp",
	"jpe",
	"jpo",
	"js",
	"jz",
	"lahf",
	"lds",
	"lea",
	"les",
	"lock",
	"lodsb",
	"lodsw",
	"loop",
	"loopnz",
	"loopz",
	"mov",
	"movsb",
	"movsw",
	"mul",
	"neg",
	"nop",
	"not",
	"or",
	"out",
	"pop",
	"popf",
	"push",
	"pushf",
	"rcl",
	"rcr",
	"repnz",
	"repz",
	"ret",
	"retf",
	"rol",
	"ror",
	"sahf",
	"sar",
	"sbb",
	"scasb",
	"scasw",
	"shl",
	"shr",
	"stc",
	"std",
	"sti",
	"stosb",
	"stosw",
	"sub",
	"test",
	"wait",
	"xchg",
	"xlat",
	"xor"
};

struct ops_s {
	e_ops name;
	const char *arg1;
	const char *arg2;
};

ops_s ops[] = {
	{ op_add,    "Eb", "Gb"  },  // 00
	{ op_add,    "Ev", "Gv"  },  // 01
	{ op_add,    "Gb", "Eb"  },  // 02
	{ op_add,    "Gv", "Ev"  },  // 03
	{ op_add,    "al", "Ib"  },  // 04
	{ op_add,    "ax", "Iv"  },  // 05
	{ op_push,   "es"        },  // 06
	{ op_pop,    "es"        },  // 07
	{ op_or,     "Eb", "Gb"  },  // 08
	{ op_or,     "Ev", "Gv"  },  // 09
	{ op_or,     "Gb", "Eb"  },  // 0a
	{ op_or,     "Gv", "Ev"  },  // 0b
	{ op_or,     "al", "Ib"  },  // 0c
	{ op_or,     "ax", "Iv"  },  // 0d
	{ op_push,   "cs"        },  // 0e
	{ op_illegal             },  // 0f
	{ op_adc,    "Eb", "Gb"  },  // 10
	{ op_adc,    "Ev", "Gv"  },  // 11
	{ op_adc,    "Gb", "Eb"  },  // 12
	{ op_adc,    "Gv", "Ev"  },  // 13
	{ op_adc,    "al", "Ib"  },  // 14
	{ op_adc,    "ax", "Iv"  },  // 15
	{ op_push,   "ss"        },  // 16
	{ op_pop,    "ss"        },  // 17
	{ op_sbb,    "Eb", "Gb"  },  // 18
	{ op_sbb,    "Ev", "Gv"  },  // 19
	{ op_sbb,    "Gb", "Eb"  },  // 1a
	{ op_sbb,    "Gv", "Ev"  },  // 1b
	{ op_sbb,    "al", "Ib"  },  // 1c
	{ op_sbb,    "ax", "Iv"  },  // 1d
	{ op_push,   "ds"        },  // 1e
	{ op_pop,    "ds"        },  // 1f
	{ op_and,    "Eb", "Gb"  },  // 20
	{ op_and,    "Ev", "Gv"  },  // 21
	{ op_and,    "Gb", "Eb"  },  // 22
	{ op_and,    "Gv", "Ev"  },  // 23
	{ op_and,    "al", "Ib"  },  // 24
	{ op_and,    "ax", "Iv"  },  // 25
	{ op_illegal             },  // 26
	{ op_daa,                },  // 27
	{ op_sub,    "Eb", "Gb"  },  // 28
	{ op_sub,    "Ev", "Gv"  },  // 29
	{ op_sub,    "Gb", "Eb"  },  // 2a
	{ op_sub,    "Gv", "Ev"  },  // 2b
	{ op_sub,    "al", "Ib"  },  // 2c
	{ op_sub,    "ax", "Iv"  },  // 2d
	{ op_illegal             },  // 2e
	{ op_das,                },  // 2f
	{ op_xor,    "Eb", "Gb"  },  // 30
	{ op_xor,    "Ev", "Gv"  },  // 31
	{ op_xor,    "Gb", "Eb"  },  // 32
	{ op_xor,    "Gv", "Ev"  },  // 33
	{ op_xor,    "al", "Ib"  },  // 34
	{ op_xor,    "ax", "Iv"  },  // 35
	{ op_illegal             },  // 36
	{ op_aaa,                },  // 37
	{ op_cmp,    "Eb", "Gb"  },  // 38
	{ op_cmp,    "Ev", "Gv"  },  // 39
	{ op_cmp,    "Gb", "Eb"  },  // 3a
	{ op_cmp,    "Gv", "Ev"  },  // 3b
	{ op_cmp,    "al", "Ib"  },  // 3c
	{ op_cmp,    "ax", "Iv"  },  // 3d
	{ op_illegal             },  // 3e
	{ op_aas,                },  // 3f
	{ op_inc,    "ax"        },  // 40
	{ op_inc,    "cx"        },  // 41
	{ op_inc,    "dx"        },  // 42
	{ op_inc,    "bx"        },  // 43
	{ op_inc,    "sp"        },  // 44
	{ op_inc,    "bp"        },  // 45
	{ op_inc,    "si"        },  // 46
	{ op_inc,    "di"        },  // 47
	{ op_dec,    "ax"        },  // 48
	{ op_dec,    "cx"        },  // 49
	{ op_dec,    "dx"        },  // 4a
	{ op_dec,    "bx"        },  // 4b
	{ op_dec,    "sp"        },  // 4c
	{ op_dec,    "bp"        },  // 4d
	{ op_dec,    "si"        },  // 4e
	{ op_dec,    "di"        },  // 4f
	{ op_push,   "ax"        },  // 50
	{ op_push,   "cx"        },  // 51
	{ op_push,   "dx"        },  // 52
	{ op_push,   "bx"        },  // 53
	{ op_push,   "sp"        },  // 54
	{ op_push,   "bp"        },  // 55
	{ op_push,   "si"        },  // 56
	{ op_push,   "di"        },  // 57
	{ op_pop,    "ax"        },  // 58
	{ op_pop,    "cx"        },  // 59
	{ op_pop,    "dx"        },  // 5a
	{ op_pop,    "bx"        },  // 5b
	{ op_pop,    "sp"        },  // 5c
	{ op_pop,    "bp"        },  // 5d
	{ op_pop,    "si"        },  // 5e
	{ op_pop,    "di"        },  // 5f
	{ op_illegal             },  // 60
	{ op_illegal             },  // 61
	{ op_illegal             },  // 62
	{ op_illegal             },  // 63
	{ op_illegal             },  // 64
	{ op_illegal             },  // 65
	{ op_illegal             },  // 66
	{ op_illegal             },  // 67
	{ op_illegal             },  // 68
	{ op_illegal             },  // 69
	{ op_illegal             },  // 6a
	{ op_illegal             },  // 6b
	{ op_illegal             },  // 6c
	{ op_illegal             },  // 6d
	{ op_illegal             },  // 6e
	{ op_illegal             },  // 6f
	{ op_jo,     "Jb"        },  // 70
	{ op_jno,    "Jb"        },  // 71
	{ op_jc,     "Jb"        },  // 72
	{ op_jnc,    "Jb"        },  // 73
	{ op_je,     "Jb"        },  // 74
	{ op_jne,    "Jb"        },  // 75
	{ op_jbe,    "Jb"        },  // 76
	{ op_ja,     "Jb"        },  // 77
	{ op_js,     "Jb"        },  // 78
	{ op_jns,    "Jb"        },  // 79
	{ op_jpe,    "Jb"        },  // 7a
	{ op_jpo,    "Jb"        },  // 7b
	{ op_jl,     "Jb"        },  // 7c
	{ op_jge,    "Jb"        },  // 7d
	{ op_jle,    "Jb"        },  // 7e
	{ op_jg,     "Jb"        },  // 7f
	{ op_grp1,   "Eb", "Ib"  },  // 80
	{ op_grp1,   "Ev", "Iv"  },  // 81
	{ op_grp1,   "Eb", "Ib"  },  // 82
	{ op_grp1,   "Ev", "Ix"  },  // 83
	{ op_test,   "Eb", "Gb"  },  // 84
	{ op_test,   "Ev", "Gv"  },  // 85
	{ op_xchg,   "Eb", "Gb"  },  // 86
	{ op_xchg,   "Ev", "Gv"  },  // 87
	{ op_mov,    "Eb", "Gb"  },  // 88
	{ op_mov,    "Ev", "Gv"  },  // 89
	{ op_mov,    "Gb", "Eb"  },  // 8a
	{ op_mov,    "Gv", "Ev"  },  // 8b
	{ op_mov,    "Ew", "Sw"  },  // 8c
	{ op_lea,    "Gv", "M"   },  // 8d
	{ op_mov,    "Sw", "Ew"  },  // 8e
	{ op_pop,    "Ev"        },  // 8f
	{ op_nop,                },  // 90
	{ op_xchg,   "cx", "ax"  },  // 91
	{ op_xchg,   "dx", "ax"  },  // 92
	{ op_xchg,   "bx", "ax"  },  // 93
	{ op_xchg,   "sp", "ax"  },  // 94
	{ op_xchg,   "bp", "ax"  },  // 95
	{ op_xchg,   "si", "ax"  },  // 96
	{ op_xchg,   "di", "ax"  },  // 97
	{ op_cbw,                },  // 98
	{ op_cwd,                },  // 99
	{ op_call,   "Ap"        },  // 9a
	{ op_wait,               },  // 9b
	{ op_pushf,              },  // 9c
	{ op_popf,               },  // 9d
	{ op_sahf,               },  // 9e
	{ op_lahf,               },  // 9f
	{ op_mov,    "al", "Ob"  },  // a0
	{ op_mov,    "ax", "Ov"  },  // a1
	{ op_mov,    "Ob", "al"  },  // a2
	{ op_mov,    "Ov", "ax"  },  // a3
	{ op_movsb,              },  // a4
	{ op_movsw,              },  // a5
	{ op_cmpsb,              },  // a6
	{ op_cmpsw,              },  // a7
	{ op_test,   "al", "Ib"  },  // a8
	{ op_test,   "ax", "Iv"  },  // a9
	{ op_stosb,              },  // aa
	{ op_stosw,              },  // ab
	{ op_lodsb,              },  // ac
	{ op_lodsw,              },  // ad
	{ op_scasb,              },  // ae
	{ op_scasw,              },  // af
	{ op_mov,    "al", "Ib"  },  // b0
	{ op_mov,    "cl", "Ib"  },  // b1
	{ op_mov,    "dl", "Ib"  },  // b2
	{ op_mov,    "bl", "Ib"  },  // b3
	{ op_mov,    "ah", "Ib"  },  // b4
	{ op_mov,    "ch", "Ib"  },  // b5
	{ op_mov,    "dh", "Ib"  },  // b6
	{ op_mov,    "bh", "Ib"  },  // b7
	{ op_mov,    "ax", "Iv"  },  // b8
	{ op_mov,    "cx", "Iv"  },  // b9
	{ op_mov,    "dx", "Iv"  },  // ba
	{ op_mov,    "bx", "Iv"  },  // bb
	{ op_mov,    "sp", "Iv"  },  // bc
	{ op_mov,    "bp", "Iv"  },  // bd
	{ op_mov,    "si", "Iv"  },  // be
	{ op_mov,    "di", "Iv"  },  // bf
	{ op_illegal             },  // c0
	{ op_illegal             },  // c1
	{ op_ret,    "Iw"        },  // c2
	{ op_ret,                },  // c3
	{ op_les,    "Gv", "Mp"  },  // c4
	{ op_lds,    "Gv", "Mp"  },  // c5
	{ op_mov,    "Eb", "Ib"  },  // c6
	{ op_mov,    "Ev", "Iv"  },  // c7
	{ op_illegal             },  // c8
	{ op_illegal             },  // c9
	{ op_retf,   "Iw"        },  // ca
	{ op_retf,               },  // cb
	{ op_int,    "3"         },  // cc
	{ op_int,    "Ib"        },  // cd
	{ op_into,               },  // ce
	{ op_iret,               },  // cf
	{ op_grp2,   "Eb", "1"   },  // d0
	{ op_grp2,   "Ev", "1"   },  // d1
	{ op_grp2,   "Eb", "cl"  },  // d2
	{ op_grp2,   "Ev", "cl"  },  // d3
	{ op_aam,    "I0"        },  // d4
	{ op_aad,    "I0"        },  // d5
	{ op_illegal             },  // d6
	{ op_xlat,               },  // d7
	{ op_illegal             },  // d8
	{ op_illegal             },  // d9
	{ op_illegal             },  // da
	{ op_illegal             },  // db
	{ op_illegal             },  // dc
	{ op_illegal             },  // dd
	{ op_illegal             },  // de
	{ op_illegal             },  // df
	{ op_loopnz, "Jb"        },  // e0
	{ op_loopz,  "Jb"        },  // e1
	{ op_loop,   "Jb"        },  // e2
	{ op_jcxz,   "Jb"        },  // e3
	{ op_in,     "al", "Ib"  },  // e4
	{ op_in,     "ax", "Ib"  },  // e5
	{ op_out,    "Ib", "al"  },  // e6
	{ op_out,    "Ib", "ax"  },  // e7
	{ op_call,   "Jv"        },  // e8
	{ op_jmp,    "Jv"        },  // e9
	{ op_jmp,    "Ap"        },  // ea
	{ op_jmp,    "Jb"        },  // eb
	{ op_in,     "al", "dx"  },  // ec
	{ op_in,     "ax", "dx"  },  // ed
	{ op_out,    "dx", "al"  },  // ee
	{ op_out,    "dx", "ax"  },  // ef
	{ op_lock,               },  // f0
	{ op_illegal             },  // f1
	{ op_repnz,              },  // f2
	{ op_repz,               },  // f3
	{ op_hlt,                },  // f4
	{ op_cmc,                },  // f5
	{ op_grp3a               },  // f6
	{ op_grp3b               },  // f7
	{ op_clc,                },  // f8
	{ op_stc,                },  // f9
	{ op_cli,                },  // fa
	{ op_sti,                },  // fb
	{ op_cld,                },  // fc
	{ op_std,                },  // fd
	{ op_grp4                },  // fe
	{ op_grp5                },  // ff
};

ops_s grp_ops[] = {
	{ op_add                 },  // grp 1
	{ op_or                  },  // grp 1
	{ op_adc                 },  // grp 1
	{ op_sbb                 },  // grp 1
	{ op_and                 },  // grp 1
	{ op_sub                 },  // grp 1
	{ op_xor                 },  // grp 1
	{ op_cmp                 },  // grp 1

	{ op_rol                 },  // grp 2
	{ op_ror                 },  // grp 2
	{ op_rcl                 },  // grp 2
	{ op_rcr                 },  // grp 2
	{ op_shl                 },  // grp 2
	{ op_shr                 },  // grp 2
	{ op_illegal             },  // grp 2
	{ op_sar                 },  // grp 2

	{ op_test,   "Eb", "Ib"  },  // grp 3a
	{ op_illegal             },  // grp 3a
	{ op_not,    "Eb"        },  // grp 3a
	{ op_neg,    "Eb"        },  // grp 3a
	{ op_mul,    "Eb"        },  // grp 3a
	{ op_imul,   "Eb"        },  // grp 3a
	{ op_div,    "Eb"        },  // grp 3a
	{ op_idiv,   "Eb"        },  // grp 3a

	{ op_test,   "Ev", "Iv"  },  // grp 3b
	{ op_illegal             },  // grp 3b
	{ op_not,    "Ev"        },  // grp 3b
	{ op_neg,    "Ev"        },  // grp 3b
	{ op_mul,    "Ev"        },  // grp 3b
	{ op_imul,   "Ev"        },  // grp 3b
	{ op_div,    "Ev"        },  // grp 3b
	{ op_idiv,   "Ev"        },  // grp 3b

	{ op_inc,    "Eb"        },  // grp 4
	{ op_dec,    "Eb"        },  // grp 4
	{ op_illegal             },  // grp 4
	{ op_illegal             },  // grp 4
	{ op_illegal             },  // grp 4
	{ op_illegal             },  // grp 4
	{ op_illegal             },  // grp 4
	{ op_illegal             },  // grp 4

	{ op_inc,    "Ev"        },  // grp 5
	{ op_dec,    "Ev"        },  // grp 5
	{ op_call,   "Ev"        },  // grp 5
	{ op_call,   "Ep"        },  // grp 5
	{ op_jmp,    "Ev"        },  // grp 5
	{ op_jmp,    "Ep"        },  // grp 5
	{ op_push,   "Ev"        },  // grp 5
	{ op_illegal             },  // grp 5
};

const char *register_names[] =
{
	"al", "cl", "dl", "bl",
	"ah", "ch", "dh", "bh",
	"ax", "cx", "dx", "bx",
	"sp", "bp", "si", "di",
	"es", "cs", "ss", "ds", "fs", "gs",
	"<noreg>"
};

const char *register_name(e_regs r)
{
	return register_names[r];
}

const char *str_prefix[] =
{
	"rep",
	"repe",
	"repne",
	"repnz",
	"lock"
};

bool is_branch(e_ops op_name)
{
	switch(op_name)
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
		case op_loop:
		case op_loopnz:
		case op_loopz:
			return true;
		default:
			return false;
	}
}

#define DEBUG 0

void x86_insn_arg::dump(char *str)
{
	const char *kinds[] = { "none", "reg", "imm", "adr", "mem" };
	const char *sizes[] = { "unk", "byte", "word", "dword" };

	if (kind != KN_NONE)
		sprintf(str, "(kn=%s,sz=%s)", kinds[kind], sizes[size]);
}

void x86_insn_arg::to_str(char *str)
{
	if (kind == KN_REG)
	{
		strcpy(str, register_names[reg]);
		return;
	}

	if (kind == KN_IMM)
	{
		sprintf(str, size == SZ_BYTE ? "%02X" : "%04X", imm);
		return;
	}

	if (kind == KN_ADR)
	{
		sprintf(str, "%04X:%04X", seg, ofs);
		return;
	}

	if (kind == KN_MEM)
	{
		strcpy(str, "[");

		bool has_prev = false;

		if (base_reg != r_none)
		{
			strcat(str, register_names[base_reg]);
			has_prev = true;
		}
		if (idx_reg != r_none)
		{
			strcat(str, "+");
			strcat(str, register_names[idx_reg]);
			has_prev = true;
		}
		if (disp)
		{
			if (!has_prev)
			{
				sprintf(str+strlen(str), disp_size == SZ_BYTE ? "%02X" : "%04X", disp);
			}
			else if (disp_size == SZ_BYTE)
			{
				bool neg = disp & 0x80;
				byte d = (neg ? -disp : disp) & 0xff;
				sprintf(str+strlen(str), "%c%02X", neg ? '-' : '+', d);
			}
			else if (disp_size == SZ_WORD)
			{
				bool neg = disp & 0x8000;
				uint16 d = (neg ? -disp : disp) & 0xffff;
				sprintf(str+strlen(str), "%c%04X", neg ? '-' : '+', d);
			}
		}

		strcat(str, "]");
	}
}

void x86_insn::dump(char *str)
{
	arg[0].dump(str+strlen(str));
	arg[1].dump(str+strlen(str));
}

void x86_insn::to_str(char *str, x86_16_address_t addr, annotations_t *annotations)
{
	str[0] = '\0';

	if (prefix != pf_none)
	{
		strcat(str, str_prefix[prefix]);
		strcat(str, " ");
	}
	if (arg[0].kind == KN_NONE)
	{
		strcat(str, str_ops[op_name]);
		return;
	}

	sprintf(str+strlen(str), "%-4s ", str_ops[op_name]);

	if (op_name == op_call)
	{
		if (arg[0].kind == KN_MEM)
		{
			const char *size_specifiers[] = { "", "<hat>", "near word ", "far word " };
			strcat(str, size_specifiers[arg[0].size]);
		}
		else
		if (arg[0].kind == KN_REG)
			strcat(str, "near ");
	}
	else
	if (op_name == op_jmp)
	{
		if (arg[0].kind == KN_IMM && arg[0].size == SZ_BYTE)
			strcat(str, "short ");
		else
		if (arg[0].kind == KN_REG)
			strcat(str, "near ");
		else
		if (arg[0].kind == KN_MEM)
			strcat(str, "near word ");
	}
	else
	{
		int size_specifier = SZ_UNK;
		if (arg[0].kind == KN_MEM && arg[1].kind != KN_REG)
			size_specifier = arg[0].size;
		else
		if (arg[1].kind == KN_MEM && arg[0].kind != KN_REG)
			size_specifier = arg[1].size;
		else
		if (arg[0].kind == KN_MEM && ops[op].name == op_grp2)
			size_specifier = arg[0].size;

		const char *size_specifiers[] = { "", "byte ", "word ", "dword " };
		strcat(str, size_specifiers[size_specifier]);
	}

	if (is_branch(op_name) && arg[0].kind == KN_IMM)
	{
		x86_16_address_t dst;

		if (annotations && x86_16_branch_destination(*this, addr, &dst))
		{
			procs_t::const_iterator pi = annotations->procs->find(dst.ea());
			if (pi != annotations->procs->end() && pi->name)
			{
				int offset = dst.ea() - pi->begin();
				sprintf(str+strlen(str), "%s", pi->name);
				if (offset)
					sprintf(str+strlen(str), "+%x", offset);

				int w = strlen(str);
				while (w < 24)
					str[w++] = ' ';
				str[w] = '\0';
				sprintf(str+strlen(str), " (%04x:%04x)", dst.seg, dst.ofs);
				return;
			}
		}

		if (arg[0].size == SZ_BYTE)
		{
			sprintf(str+strlen(str), "%08X", (int8)(arg[0].imm + op_size));
			sprintf(str+strlen(str), " ($%c%x)", arg[0].imm & 0x80 ? '-' : '+', (arg[0].imm & 0x80 ? -arg[0].imm : arg[0].imm) & 0xff);
		}
		else
		{
			sprintf(str+strlen(str), "%08X", (int16)(arg[0].imm + op_size));
			sprintf(str+strlen(str), " ($%c%x)", arg[0].imm & 0x8000 ? '-' : '+', (arg[0].imm & 0x8000 ? -arg[0].imm : arg[0].imm) & 0xffff);
		}
		return;
	}


	char arg_str[64];
	arg_str[0] = '\0';

	for (int i = 0; i != 2; ++i)
	{
		if (arg[i].kind != KN_NONE)
		{
			arg[i].to_str(arg_str);

			if (i > 0)
				strcat(str, ",");

			if (i == 1 && ops[op].arg2 && isdigit(ops[op].arg2[0]))
			{
				strcat(str, ops[op].arg2);
			}
			else
			{
				if (arg[i].kind == KN_MEM && seg_ovr != r_none)
				{
					strcat(str, register_names[seg_ovr]);
					strcat(str, ":");
				}

				strcat(str, arg_str);
			}
		}
	}
}

class x86_decoder
{
	byte op;
	byte modrm;
	bool has_modrm;

	x86_insn insn;

	byte read_byte(const byte *&p)
	{
		if (DEBUG) printf("%02x ", *p);
		return *p++;
	}

	uint16 read_uint16(const byte *&p)
	{
		uint16 lo = read_byte(p);
		uint16 hi = read_byte(p);
		return (hi << 8) + lo;
	}

	void do_modrm_rm(const byte *&p, const char *arg_spec, x86_insn_arg &arg);
	void do_arg(const byte *&p, const char *arg_spec, x86_insn_arg &arg);

	bool is_memory_dereference(const char *arg_spec);
	bool is_register(const char *arg_spec);

public:
	x86_decoder(const byte *ap);
	x86_insn get_insn()
	{
		return insn;
	}
};

x86_decoder::x86_decoder(const byte *p)
	: has_modrm(false)
{
	const byte *orig_p = p;
	insn.prefix = pf_none;

	switch (*p)
	{
		case 0xf0: // LOCK
			insn.prefix = pf_lock;
			++p;
			break;
		case 0xf2: // REPNE
			insn.prefix = pf_repne;
			++p;
			break;
		case 0xf3: // REPE
			insn.prefix = pf_repe;
			++p;
			break;
		default:;
	}

	insn.seg_ovr = r_none;
	switch (*p)
	{
		case 0x26: // ES
			insn.seg_ovr = r_es;
			++p;
			break;
		case 0x2e: // CS
			insn.seg_ovr = r_cs;
			++p;
			break;
		case 0x36: // SS
			insn.seg_ovr = r_ss;
			++p;
			break;
		case 0x3e: // DS
			insn.seg_ovr = r_ds;
			++p;
			break;
/*
		case 0x64: // FS
			insn.seg_ovr = r_fs;
			++p;
			break;
		case 0x65: // GS
			insn.seg_ovr = r_gs;
			++p;
			break;
*/
		default:;
	};

	if (DEBUG)
		printf("seg_ovr=%s ", register_names[insn.seg_ovr]);

	byte op = read_byte(p);
	insn.op = op;

	if (DEBUG)
		printf("(%02x)", op);

	const char *arg_spec1;
	const char *arg_spec2;

	int grp;
	switch (ops[op].name)
	{
		case op_grp1:  grp = 0; break;
		case op_grp2:  grp = 1; break;
		case op_grp3a: grp = 2; break;
		case op_grp3b: grp = 3; break;
		case op_grp4:  grp = 4; break;
		case op_grp5:  grp = 5; break;
		//case op_grp6:  grp = 6; break;
		//case op_grp7:  grp = 7; break;
		//case op_grp8:  grp = 8; break;
		default:
			grp = -1;
	}

	if (grp == -1)
	{
		insn.op_name = ops[op].name;
		arg_spec1 = ops[op].arg1;
		arg_spec2 = ops[op].arg2;
	} else {
		modrm = read_byte(p);
		has_modrm = true;

		byte reg = (modrm >> 3) & 0x07;

		if (DEBUG)
			printf("(reg=%d)", reg);

		insn.op_name = grp_ops[8*grp + reg].name;
		arg_spec1 = grp_ops[8*grp + reg].arg1;
		arg_spec2 = grp_ops[8*grp + reg].arg2;

		if (!arg_spec1) arg_spec1 = ops[op].arg1;
		if (!arg_spec2) arg_spec2 = ops[op].arg2;
	}

	do_arg(p, arg_spec1, insn.arg[0]);
	do_arg(p, arg_spec2, insn.arg[1]);

	insn.op_size = p - orig_p;
}

void x86_decoder::do_modrm_rm(const byte *&p, const char *arg_spec, x86_insn_arg &arg)
{
	if (!has_modrm)
		modrm = read_byte(p);
	has_modrm = true;

	byte mod = (modrm >> 6) & 0x03;
	byte rm  = (modrm >> 0) & 0x07;

	arg.kind = KN_MEM;
	arg.base_reg = r_none;
	arg.idx_reg  = r_none;
	arg.disp_size = SZ_UNK;
	arg.disp     = 0;

	if (DEBUG)
		printf("(mod=%d,rm=%d)", mod, rm);

	if (mod == 0 && rm == 0x06)
	{
		arg.disp_size = SZ_WORD;
		arg.disp = read_uint16(p);
		return;
	}

	if (mod == 3)
	{
		arg.kind = KN_REG;
		arg.reg  = rm + (arg.size == SZ_BYTE ? 0 : 8);
		return;
	}

	switch (rm)
	{
		case 0:
			arg.base_reg = r_bx;
			arg.idx_reg  = r_si;
			break;
		case 1:
			arg.base_reg = r_bx;
			arg.idx_reg  = r_di;
			break;
		case 2:
			arg.base_reg = r_bp;
			arg.idx_reg  = r_si;
			break;
		case 3:
			arg.base_reg = r_bp;
			arg.idx_reg  = r_di;
			break;
		case 4:
			arg.base_reg = r_si;
			break;
		case 5:
			arg.base_reg = r_di;
			break;
		case 6:
			arg.base_reg = r_bp;
			break;
		case 7:
			arg.base_reg = r_bx;
			break;
	}

	if (mod == 1)
	{
		arg.disp_size = SZ_BYTE;
		arg.disp = read_byte(p);
	}
	else if (mod == 2)
	{
		arg.disp_size = SZ_WORD;
		arg.disp = read_uint16(p);
	}
}

bool x86_decoder::is_memory_dereference(const char *arg_spec)
{
	if (arg_spec[0] == 'M' || arg_spec[0] == 'A')
		return true;

	if (arg_spec[0] != 'E')
		return false;

	byte mod = (modrm >> 6) & 0x03;
	return mod != 0x03;
}

bool x86_decoder::is_register(const char *arg_spec)
{
	if (arg_spec[0] == 'C' || // the reg field of modrm selects control register
	    arg_spec[0] == 'D' || // the reg field of modrm selects debug register
	    arg_spec[0] == 'F' || // flags register
	    arg_spec[0] == 'G' || // the reg field of modrm selects general register
	    arg_spec[0] == 'R' || // the mod field of modrm selects general register
	    arg_spec[0] == 'S' || // the reg field of modrm selects segment register
	    arg_spec[0] == 'T'    // the reg field of modrm selects test register
	   )
	{
		return true;
	}

	if (islower(arg_spec[0]))
		return true;

	if (arg_spec[0] != 'E')
	{
		byte mod = (modrm >> 6) & 0x03;
		return mod == 0x03;
	}

	return false;
}

static byte sizespec_to_size(char ss)
{
	switch (ss)
	{
	case 'b':
		return SZ_BYTE;
	case 'v':
	case 'w':
		return SZ_WORD;
	case 'p':
		return SZ_DWORD;
	default:
		assert(false);
	}
}

void x86_decoder::do_arg(const byte *&p, const char *arg_spec, x86_insn_arg &arg)
{
	arg.kind = KN_NONE;

	if (!arg_spec)
		return;

	if (DEBUG) printf("%s ", arg_spec);

	if (!isupper(arg_spec[0]))
	{
		if (isdigit(arg_spec[0]))
		{
			arg.kind = KN_IMM;
			arg.imm  = strtol(arg_spec, NULL, 10);
			return;
		}

		int i;
		for (i = r_al; i != r_none; ++i)
			if (strcmp(arg_spec, register_names[i]) == 0)
				break;

		if (i != r_none)
		{
			arg.kind = KN_REG;
			arg.reg  = (e_regs)i;
			arg.size = arg.reg < 8 ? SZ_BYTE : SZ_WORD;
			return;
		}

		printf("Unrecognized arg_spec '%s'", arg_spec);
		exit(1);
		return;
	}

	switch (arg_spec[0])
	{
	case 'A': {
		uint16 ofs = read_uint16(p);
		uint16 seg = read_uint16(p);
		arg.kind = KN_ADR;
		arg.size = SZ_DWORD;
		arg.seg = seg;
		arg.ofs = ofs;
		break;
	}
	case 'E': {
		arg.size = sizespec_to_size(arg_spec[1]);
		do_modrm_rm(p, arg_spec, arg);
		break;
	}
	case 'G': {
		if (!has_modrm)
			modrm = read_byte(p);
		has_modrm = true;

		byte reg = (modrm >> 3) & 0x07;

		arg.kind = KN_REG;
		arg.reg  = reg + (arg_spec[1] == 'b' ? 0 : 8);
		arg.size = arg.reg < 8 ? SZ_BYTE : SZ_WORD;
		break;
	}
	case 'M': {
		do_modrm_rm(p, arg_spec, arg);
		assert(arg.kind == KN_MEM);

		break;
	}
	case 'I':
	case 'J': {
		arg.kind = KN_IMM;
		if (arg_spec[1] == 'x')
		{
			arg.size = SZ_WORD;
			arg.imm = read_byte(p);
			arg.imm |= (arg.imm & 0x80) ? 0xff00 : 0x0000;
		}
		else
		{
			arg.size = (arg_spec[1] == 'b') ? SZ_BYTE : SZ_WORD;
			arg.imm  = (arg_spec[1] == 'b') ? read_byte(p) : read_uint16(p);
		}
		break;
	}
	case 'O': {
		arg.kind = KN_MEM;
		arg.size = SZ_UNK;
		arg.base_reg = r_none;
		arg.idx_reg  = r_none;
		arg.disp_size = SZ_WORD;
		arg.disp = read_uint16(p);
		break;
	}
	case 'S': {
		if (!has_modrm)
			modrm = read_byte(p);
		has_modrm = true;

		byte reg = (modrm >> 3) & 0x07;

		arg.kind = KN_REG;
		arg.size = SZ_WORD;
		arg.reg  = reg + 16;
		break;
	}
	default:
		puts("BLAH!");
		exit(1);
	}
}

x86_insn x86_decode(const byte *p)
{
	x86_decoder decoder(p);
	return decoder.get_insn();
}
