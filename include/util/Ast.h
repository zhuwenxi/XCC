#ifndef __AST_H__
#define __AST_H__

#include "util/array_list.h"
#include "util/context_free_grammar.h"

#include <stdarg.h>

typedef struct
{
	char *desc;
	production_token_type symbol;
} Ast_operator_type;

typedef struct
{
	char *desc;
	production_token_type symbol;
} Ast_operand_type;

typedef struct
{
	Ast_operator_type *operator;
	array_list_type *operands;
} Ast_node_type;

typedef struct
{
	Ast_node_type *root;
} Ast_type;


/*
 * constructor & de-constuctor:
 */
Ast_type *Ast_create();

bool Ast_deconstructor(Ast_type *ast, va_list arg_list);
DECLARE_DESTROY(Ast);

Ast_node_type *Ast_node_create();

bool Ast_node_deconstructor(Ast_node_type *node, va_list arg_list);
DECLARE_DESTROY(Ast_node);

Ast_operator_type *Ast_operator_create();

bool Ast_operator_deconstructor(Ast_operator_type *operator, va_list arg_list);
DECLARE_DESTROY(Ast_operator);

/*
 * Ast operations:
 */

void Ast_set_operator(Ast_type *ast, Ast_operator_type *operator);

void Ast_set_operand(Ast_type *ast, int index, Ast_operand_type *operand);

void Ast_add_operand(Ast_type *ast, Ast_operand_type *operand);

#endif