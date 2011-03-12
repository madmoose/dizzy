#ifndef INST_H
#define INST_H

#include "base/base.h"

struct insn_t;

struct insn_arg_t;
struct insn_arg_reg_t;
struct insn_arg_imm_t;
struct insn_arg_mem_t;

struct insn_visitor_t;
struct insn_arg_visitor_t;


// Define the visitors first so the accept-impl can be inline

struct insn_visitor_t
{
	virtual void visit(insn_t *) = 0;
};

struct insn_arg_visitor_t
{
	virtual void visit(insn_arg_t *) = 0;
	virtual void visit(insn_arg_reg_t *) = 0;
	virtual void visit(insn_arg_imm_t *) = 0;
	virtual void visit(insn_arg_mem_t *) = 0;
};

struct insn_t
{
	uint             op;
	insn_arg_t      *args;
	range_t<uint32>  src_range;

	virtual void accept(insn_visitor_t *v) { v->visit(this); }
};

#define visitor_impl void accept(insn_arg_visitor_t *v) { v->visit(this); }

struct insn_arg_t
{
	insn_arg_t *next;

	virtual void accept(insn_arg_visitor_t *v) { v->visit(this); }
};

struct insn_arg_reg_t : insn_arg_t
{
	uint32_t reg;

	visitor_impl;
};

struct insn_arg_imm_t : insn_arg_t
{
	uint64_t value;
	byte     size; // in bits

	visitor_impl;
};

struct insn_arg_mem_t : insn_arg_t
{
	insn_arg_t *address;
	byte        size; // in bits

	visitor_impl;
};

#undef visitor_impl

#endif
