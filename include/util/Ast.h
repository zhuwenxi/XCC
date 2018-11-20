#ifndef __AST_H__
#define __AST_H__

#include "util/array_list.h"
#include "util/context_free_grammar.h"

#include <stdarg.h>

#define OPERATOR_NODE(node) (node->type.operator_node)
#define OPERAND_NODE(node) (node->type.operand)

typedef struct
{
	char *desc;
	production_token_type type;
} Ast_operator_type, Ast_operand_type;

typedef struct
{
	Ast_operator_type *operator;
	array_list_type *operand_nodes;
} Ast_operator_node_type;

typedef struct
{
	union {
		Ast_operator_node_type *operator_node;
		Ast_operand_type *operand;
	} type;
	bool is_operator_node;
	int id;
} Ast_node_type;

extern int ast_node_id;

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

Ast_node_type *Ast_node_create(bool is_operator_node, char *desc, char symbol);

bool Ast_node_deconstructor(Ast_node_type *node, va_list arg_list);
DECLARE_DESTROY(Ast_node);

Ast_operator_node_type *Ast_operator_node_create();

bool Ast_operator_node_deconstructor(Ast_operator_node_type *node, va_list arg_list);
DECLARE_DESTROY(Ast_operator_node);

Ast_operand_type *Ast_operand_create();

bool Ast_operand_deconstructor(Ast_operand_type *operand, va_list arg_list);
DECLARE_DESTROY(Ast_operand);

#define Ast_operator_create Ast_operand_create
#define Ast_operator_deconstructor Ast_operand_deconstructor
DECLARE_DESTROY(Ast_operator);


/*
 * Ast operations:
 */
void Ast_set_sub_node(Ast_node_type *node, int i, Ast_node_type *sub_node);
void Ast_append_sub_node(Ast_node_type *node, Ast_node_type *sub_node);

char *Ast_node_debug_str(Ast_node_type *node, va_list arg_list);
char *get_Ast_node_debug_str(Ast_node_type *node, ...);

char *Ast_debug_str(Ast_type *ast, va_list arg_list);
char *get_Ast_debug_str(Ast_type *ast, ...);

#endif