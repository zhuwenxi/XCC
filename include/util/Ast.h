#ifndef __AST_H__
#define __AST_H__

#include "util/array_list.h"
#include "util/context_free_grammar.h"

#include <stdarg.h>

typedef struct
{
	char *desc;
	production_token_type symbol;
} Ast_operator_type, Ast_operand_type;

typedef struct
{
	Ast_operator_type *operator;
	array_list_type *children;
} Ast_operator_node_type;

// typedef struct
// {
// 	Ast_operand_type *operand;
// } Ast_operand_node_type;

typedef struct
{
	union {
		Ast_operator_node_type *operator_node;
		Ast_operand_type *operand;
	} type;
	bool is_operator_node;
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

Ast_operator_node_type *Ast_operator_node_create();

bool Ast_operator_node_deconstructor(Ast_operator_node_type *node, va_list arg_list);
DECLARE_DESTROY(Ast_operator_node);

Ast_operand_type *Ast_operand_create();

bool Ast_operand_deconstructor(Ast_operand_type *operand, va_list arg_list);
DECLARE_DESTROY(Ast_operand);

#define Ast_operator_type Ast_operand_type
#define Ast_operator_deconstructor Ast_operand_deconstructor
DECLARE_DESTROY(Ast_operator);


/*
 * Ast operations:
 */

void Ast_node_set_operator_node(Ast_node_type *node, int i, Ast_operator_node_type *operator_node);
void Ast_operator_node_append_operator_node(Ast_node_type *node, Ast_operator_node_type *operator_node);

void Ast_node_set_operand(Ast_node_type *node, int i, Ast_operand_type operand);
void Ast_node_append_operand(Ast_node_type *node, Ast_operand_type operand);

char *Ast_node_debug_str(Ast_node_type *node, va_list arg_list);
char *get_Ast_node_debug_str(Ast_node_type *node, ...);

char *Ast_debug_str(Ast_type *ast, va_list arg_list);
char *get_Ast_debug_str(Ast_type *ast, ...);

#endif