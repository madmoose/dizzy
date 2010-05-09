#include "register_trace.h"
#include <cassert>
#include <cstring>

register_trace_t::register_trace_t()
{
	clear_all();
}

void register_trace_t::clear_all()
{
	for (int r = 0; r != 16; ++r)
		reg_known[r] = false;
}

void register_trace_t::clear_register(e_regs r)
{
	if (r < 8)
		reg_known[r] = false;
	else if (r < 12)
		reg_known[r - 8] = reg_known[r - 4] = false;
	else
		reg_known[r - 4] = false;
}

bool register_trace_t::is_register_known(e_regs r)
{
	if (r < 8)
		return reg_known[r];
	if (r < 12)
		return reg_known[r - 8] && reg_known[r - 4];

	return reg_known[r - 4];
}

void register_trace_t::set_register(e_regs r, uint16 v)
{
	if (r < 8)
		assert(v >> 8 == 0);

	if (r < 8)
		reg_known[r] = true;
	else if (r < 12)
		reg_known[r - 8] = reg_known[r - 4] = true;
	else
		reg_known[r - 4] = true;

	if (r < 4)
		regs[r] = (regs[r] & 0xff00) | v;
	else if (r < 8)
		regs[r - 4] = (regs[r - 4] & 0xff) | (v << 8);
	else
		regs[r - 8] = v;
}

uint16 register_trace_t::get_register(e_regs r)
{
	assert(is_register_known(r));

	if (r < 4)
		return regs[r] & 0xff;
	if (r < 8)
		return regs[r - 4] >> 8;

	return regs[r - 8];
}

void register_trace_t::dump(char *s)
{
	char gen_regs[4] = { 'a', 'c', 'd', 'b' };
	sprintf(s+strlen(s), "{ ");
	bool sep = false;

	for (int i = 0; i != 4; ++i)
	{
		bool l_known = reg_known[r_al + i];
		bool h_known = reg_known[r_ah + i];

		if (h_known && l_known)
			sprintf(s+strlen(s), "%s%cx=%04x", sep ? ", " : "", gen_regs[i], regs[i]), sep = true;
		else if (h_known)
			sprintf(s+strlen(s), "%s%ch=%02x", sep ? ", " : "", gen_regs[i], regs[i] >> 8), sep = true;
		else if (l_known)
			sprintf(s+strlen(s), "%s%cl=%02x", sep ? ", " : "", gen_regs[i], regs[i] & 0xff), sep = true;
	}

	for (int i = r_sp; i <= r_ds; ++i)
		if (is_register_known((e_regs)i))
			sprintf(s+strlen(s), "%s%s=%04x", sep ? ", " : "", register_name((e_regs)i), get_register((e_regs)i)), sep = true;

	sprintf(s+strlen(s), " }");

	if (!sep)
		*s = '\0';
}

void register_trace_t::apply_insn(x86_insn &insn)
{
	switch (insn.op_name) {
	case op_mov:   apply_op_mov(insn);   break;
	case op_push:  apply_op_push(insn);   break;

	case op_add:   apply_op_add(insn);   break;
	case op_sub:   apply_op_sub(insn);   break;
	case op_xor:   apply_op_xor(insn);   break;

	default:
		clear_all();
	}
}

bool register_trace_t::is_value_known(const x86_insn &insn, int arg_i, uint16 &val)
{
	if (insn.arg[arg_i].kind == KN_IMM)
	{
		val = insn.arg[arg_i].imm;
		return true;
	}

	if (insn.arg[arg_i].kind == KN_REG && is_register_known((e_regs)insn.arg[arg_i].reg))
	{
		val = get_register((e_regs)insn.arg[arg_i].reg);
		return true;
	}

	return false;
}

void register_trace_t::apply_op_mov(x86_insn &insn)
{
	uint16 val;

	if (insn.arg[0].kind == KN_REG)
	{
		if (is_value_known(insn, 1, val))
			set_register((e_regs)insn.arg[0].reg, val);
		else
			clear_register((e_regs)insn.arg[0].reg);
	}
}

void register_trace_t::apply_op_push(x86_insn &insn)
{
	clear_register(r_sp);
}

void register_trace_t::apply_op_sub(x86_insn &insn)
{
	uint16 val0, val1;

	if (insn.arg[0].kind != KN_REG)
		return;

	if (insn.arg[1].kind == KN_REG &&
	    insn.arg[0].reg == insn.arg[1].reg)
	{
		set_register((e_regs)insn.arg[0].reg, 0);
	}
	else if (is_value_known(insn, 0, val0) &&
	         is_value_known(insn, 1, val1))
	{
		set_register((e_regs)insn.arg[0].reg, val0 - val1);
	}
	else
	{
		clear_register((e_regs)insn.arg[0].reg);
	}
}

void register_trace_t::apply_op_add(x86_insn &insn)
{
	uint16 val0, val1;

	if (insn.arg[0].kind != KN_REG)
		return;

	if (insn.arg[1].kind == KN_REG &&
	    insn.arg[0].reg == insn.arg[1].reg)
	{
		set_register((e_regs)insn.arg[0].reg, 0);
	}
	else if (is_value_known(insn, 0, val0) &&
	         is_value_known(insn, 1, val1))
	{
		set_register((e_regs)insn.arg[0].reg, val0 + val1);
	}
	else
	{
		clear_register((e_regs)insn.arg[0].reg);
	}
}

void register_trace_t::apply_op_xor(x86_insn &insn)
{
	if (insn.arg[0].kind != KN_REG)
		return;

	if (insn.arg[1].kind == KN_REG &&
	    insn.arg[0].reg == insn.arg[1].reg)
	{
		set_register((e_regs)insn.arg[0].reg, 0);
	}
	else
		clear_register((e_regs)insn.arg[0].reg);
}
