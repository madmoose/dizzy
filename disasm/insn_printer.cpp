#include "insn.h"

struct insn_printer_t : insn_visitor_t, insn_arg_visitor_t
{
	void visit(insn_t *);
	void visit(insn_arg_t *);
	void visit(insn_arg_reg_t *);
	void visit(insn_arg_imm_t *);
	void visit(insn_arg_mem_t *);
};

void insn_print(insn_t *insn)
{
	static insn_printer_t insn_printer;

	insn->accept(&insn_printer);
	putchar('\n');
}

void insn_printer_t::visit(insn_t *n)
{
	printf("insn ");

	insn_arg_t *a = n->args;
	while (a) {
		a->accept(this);
		a = a->next;
		if (a) putchar(',');
	}
}

void insn_printer_t::visit(insn_arg_t *n)
{
	printf("arg");
}

void insn_printer_t::visit(insn_arg_reg_t *n)
{
	printf("reg");
}

void insn_printer_t::visit(insn_arg_imm_t *n)
{
	printf("imm");
}

void insn_printer_t::visit(insn_arg_mem_t *n)
{
	printf("mem");
}
