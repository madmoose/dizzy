#ifndef REGISTER_TRACE_H
#define REGISTER_TRACE_H

#include "ioutils.h"
#include "x86/x86_disasm.h"

struct register_trace_t
{
	bool reg_known[16]; // order: al, cl, dl, bl, ah, ch, dh, bh
	                    //        sp, bp, si, di, es, cs, ss, ds

	uint16 regs[12];    // order: ax, cx, dx, bx
	                    //        sp, bp, si, di, es, cs, ss, ds

	register_trace_t();

	void clear_all();
	void clear_register(e_regs r);

	bool is_register_known(e_regs r);
	bool is_value_known(const x86_insn &insn, int arg_i, uint16 &val);

	void   set_register(e_regs r, uint16 v);
	uint16 get_register(e_regs r);

	void dump(char *s);

	void apply_insn(x86_insn &insn);

	void apply_op_mov(x86_insn &insn);
	void apply_op_push(x86_insn &insn);

	void apply_op_add(x86_insn &insn);
	void apply_op_sub(x86_insn &insn);
	void apply_op_xor(x86_insn &insn);
};

#endif
