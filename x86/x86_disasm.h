#ifndef X86_DISASM_H
#define X86_DISASM_H

#include "../base/base.h"

enum e_ops {
	op_illegal,
	op_aaa,
	op_aad,
	op_aam,
	op_aas,
	op_adc,
	op_add,
	op_and,
	op_call,
	op_cbw,
	op_clc,
	op_cld,
	op_cli,
	op_cmc,
	op_cmp,
	op_cmpsb,
	op_cmpsw,
	op_cwd,
	op_daa,
	op_das,
	op_dec,
	op_div,
	op_grp1,
	op_grp2,
	op_grp3a,
	op_grp3b,
	op_grp4,
	op_grp5,
	op_grp6,
	op_grp7,
	op_grp8,
	op_hlt,
	op_idiv,
	op_imul,
	op_in,
	op_inc,
	op_int,
	op_into,
	op_iret,
	op_ja,
	op_jae,
	op_jb,
	op_jbe,
	op_jc,
	op_jcxz,
	op_je,
	op_jg,
	op_jge,
	op_jl,
	op_jle,
	op_jmp,
	op_jna,
	op_jnae,
	op_jnb,
	op_jnbe,
	op_jnc,
	op_jne,
	op_jnge,
	op_jnl,
	op_jnle,
	op_jno,
	op_jnoe,
	op_jnp,
	op_jns,
	op_jnz,
	op_jo,
	op_jp,
	op_jpe,
	op_jpo,
	op_js,
	op_jz,
	op_lahf,
	op_lds,
	op_lea,
	op_les,
	op_lock,
	op_lodsb,
	op_lodsw,
	op_loop,
	op_loopnz,
	op_loopz,
	op_mov,
	op_movsb,
	op_movsw,
	op_mul,
	op_neg,
	op_nop,
	op_not,
	op_or,
	op_out,
	op_pop,
	op_popf,
	op_push,
	op_pushf,
	op_rcl,
	op_rcr,
	op_repnz,
	op_repz,
	op_ret,
	op_retf,
	op_rol,
	op_ror,
	op_sahf,
	op_sar,
	op_sbb,
	op_scasb,
	op_scasw,
	op_shl,
	op_shr,
	op_stc,
	op_std,
	op_sti,
	op_stosb,
	op_stosw,
	op_sub,
	op_test,
	op_wait,
	op_xchg,
	op_xlat,
	op_xor

};

enum e_regs
{
	r_al, r_cl, r_dl, r_bl,
	r_ah, r_ch, r_dh, r_bh,
	r_ax, r_cx, r_dx, r_bx,
	r_sp, r_bp, r_si, r_di,
	r_es, r_cs, r_ss, r_ds, //r_fs, r_gs,
	r_none
};

const char *register_name(e_regs r);

enum e_prefix
{
	pf_rep,
	pf_repe,
	pf_repne,
	pf_repnz,
	pf_lock,
	pf_none
};

#define KN_NONE  0
#define KN_REG   1
#define KN_IMM   2
#define KN_ADR   3
#define KN_MEM   4

#define SZ_UNK   0
#define SZ_BYTE  1
#define SZ_WORD  2
#define SZ_DWORD 3

struct x86_insn_arg
{
	byte kind:3;
	byte size:2;
	union
	{
		byte   reg;
		uint32 imm;
		struct
		{
			uint16 seg;
			uint16 ofs;
		};
		struct {
			byte   seg_ovr;
			byte   base_reg;
			byte   idx_reg;
			byte   disp_size;
			uint16 disp;
		};
	};

	x86_insn_arg()
		: kind(KN_NONE), size(SZ_UNK)
	{}

	void to_str(char *str);
	void dump(char *str);
};

struct x86_insn
{
	short    op_size;
	e_prefix prefix;
	e_regs   seg_ovr;
	byte     op;
	e_ops    op_name;

	x86_insn_arg arg[2];

	void to_str(char *str);
	void dump(char *str);
};

x86_insn x86_decode(const byte *p);

#endif
