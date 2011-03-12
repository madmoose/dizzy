#include "inst.h"

#define visit_if(x) if (x) x->accept(this)

struct insn_printer_t : insn_visitor_t, insn_arg_visitor_t
{
	void visit(insn_t *);
	void visit(insn_arg_t *);
	void visit(insn_arg_reg_t *);
	void visit(insn_arg_imm_t *);
	void visit(insn_arg_mem_t *);
};

void insn_printer_t::visit(insn_t *n)
{
	printf("insn ");
	visit_if(n->args);
}

void insn_printer_t::visit(insn_arg_t *n)
{
	printf("arg");
	visit_if(n->next);
}

void insn_printer_t::visit(insn_arg_reg_t *n)
{
	printf("reg");
	visit_if(n->next);
}

void insn_printer_t::visit(insn_arg_imm_t *n)
{
	printf("imm");
	visit_if(n->next);
}

void insn_printer_t::visit(insn_arg_mem_t *n)
{
	printf("mem");
	visit_if(n->next);
}
