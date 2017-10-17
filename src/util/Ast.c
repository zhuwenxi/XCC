#include "Ast.h"
#include "string_buffer.h"
#include "util/logger.h"
#include "string.h"

#include <stdlib.h>
#include <assert.h>

//
// AST:
//

Ast_type *
Ast_create()
{
	Ast_type *ast = (Ast_type *)malloc(sizeof(Ast_type));

	ast->root = NULL;

	return ast;
}

bool
Ast_deconstructor(Ast_type *ast, va_list arg_list)
{
	bool root_desctoryed = Ast_node_destroy(ast->root, arg_list);

	free(ast);

	return root_desctoryed;
}




//
// AST node:
//

Ast_node_type *
Ast_node_create(bool is_operator_node, char *desc, int symbol)
{
	Ast_node_type *node = (Ast_node_type *)malloc(sizeof(Ast_node_type));

	node->is_operator_node = is_operator_node;

	if (is_operator_node)
	{
		node->type.operator_node = Ast_operator_node_create();
		node->type.operator_node->operator->desc = desc;
		node->type.operator_node->operator->symbol = symbol;
	}
	else
	{
		node->type.operand = Ast_operand_create();
		node->type.operand->desc = desc;
		node->type.operand->symbol = symbol;
	}

	return node;
}

bool
Ast_node_deconstructor(Ast_node_type *node, va_list arg_list)
{
	bool node_destroyed = FALSE;
	if (node->is_operator_node)
	{
		node_destroyed = Ast_operator_node_destroy(node->type.operator_node);
	}
	else
	{
		node_destroyed = Ast_operand_destroy(node->type.operand);
	}
	
	free(node);

	return node_destroyed;
}




//
// operator node:
//

Ast_operator_node_type *
Ast_operator_node_create()
{
	Ast_operator_node_type *node = (Ast_operator_node_type *)malloc(sizeof(Ast_operator_node_type));

	node->operator = Ast_operator_create();
	node->operand_nodes = array_list_create();

	return node;
}

bool
Ast_operator_node_deconstructor(Ast_operator_node_type *node, va_list arg_list)
{
	free(node->operator);
	array_list_destroy(node->operand_nodes, Ast_node_deconstructor, NULL);

	return TRUE;
}




//
// operand & opeartor:
//

Ast_operand_type *
Ast_operand_create()
{
	Ast_operand_type *operand = (Ast_operand_type *)malloc(sizeof(Ast_operand_type));

	operand->desc = NULL;
	operand->symbol = -1;

	return operand;
}

bool
Ast_operand_deconstructor(Ast_operand_type *operand, va_list arg_list)
{
	free(operand);

	return TRUE;
}

void
Ast_set_sub_node(Ast_node_type *node, int i, Ast_node_type *sub_node)
{
	// call this function implies that current node is an "Operator" node.
	node->is_operator_node = TRUE;

	Ast_operator_node_type *operator_node = node->type.operator_node;
	if (operator_node->operand_nodes == NULL)
	{
		operator_node->operand_nodes = array_list_create();
	}

	array_list_set(operator_node->operand_nodes, i, sub_node);
}

void
Ast_append_sub_node(Ast_node_type *node, Ast_node_type *sub_node)
{
		// call this function implies that current node is an "Operator" node.
	node->is_operator_node = TRUE;

	Ast_operator_node_type *operator_node = node->type.operator_node;
	if (operator_node->operand_nodes == NULL)
	{
		operator_node->operand_nodes = array_list_create();
	}

	array_list_append(operator_node->operand_nodes, sub_node);
}

char *
get_Ast_debug_str(Ast_type *ast, ...)
{
	return Ast_debug_str(ast, NULL);
}

char *
Ast_debug_str(Ast_type *ast, va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, "digraph AST { ");
	if (ast->root)
	{
		string_buffer root_node_debug_str = get_Ast_node_debug_str(ast->root);
		string_buffer_append(&debug_str, root_node_debug_str);
		string_buffer_destroy(root_node_debug_str);
	}
	string_buffer_append(&debug_str, " }");

	return debug_str;
}

char *
get_Ast_node_debug_str(Ast_node_type *node, ...)
{
	return Ast_node_debug_str(node, NULL);
}

/*
 * output a dot graph string for AST's nodes.
 */
char *
Ast_node_debug_str(Ast_node_type *node, va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	if (node->is_operator_node)
	{
		assert(node->type.operator_node);
		Ast_operator_node_type *operator_node = node->type.operator_node;
		char *current_node_desc = operator_node->operator->desc;

		array_list_type *operand_nodes = operator_node->operand_nodes;
		int i;
		for (i = 0; i < operand_nodes->length; i ++)
		{
			Ast_node_type *sub_node = array_list_get(operand_nodes, i);
			assert(sub_node);

			string_buffer sub_node_debug_str = string_buffer_create();

			if (sub_node->is_operator_node)
			{
				string_buffer_append(&sub_node_debug_str, "\"");
				string_buffer_append(&sub_node_debug_str, sub_node->type.operator_node->operator->desc);
				string_buffer_append(&sub_node_debug_str, "\"");
				string_buffer_append(&sub_node_debug_str, "; ");

				string_buffer sub_operator_debug_str = Ast_node_debug_str(sub_node, NULL);
				string_buffer_append(&sub_node_debug_str, sub_operator_debug_str);

				string_buffer_destroy(sub_operator_debug_str);
			}
			else
			{
				string_buffer_append(&sub_node_debug_str, "\"");
				string_buffer_append(&sub_node_debug_str, sub_node->type.operand->desc);
				string_buffer_append(&sub_node_debug_str, "\"");
				string_buffer_append(&sub_node_debug_str, "; ");
			}

			string_buffer_append(&debug_str, "\"");
			string_buffer_append(&debug_str, current_node_desc);
			string_buffer_append(&debug_str, "\"");
			string_buffer_append(&debug_str, " -> ");
			string_buffer_append(&debug_str, sub_node_debug_str);
			// string_buffer_append(&debug_str, "; ");

			string_buffer_destroy(sub_node_debug_str);
		}
	}
	else
	{
		assert(node->type.operand);
		// if this node is pure oprand, just ignore it.
		string_buffer_append(&debug_str, node->type.operand->desc);
	}
	
	return debug_str;
}





