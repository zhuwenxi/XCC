#include "Ast.h"
#include "string_buffer.h"

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
Ast_node_create()
{
	Ast_node_type *node = (Ast_node_type *)malloc(sizeof(Ast_node_type));

	node->is_operator_node = FALSE;

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

	node->operator = NULL;
	node->children = array_list_create();

	return node;
}

bool
Ast_operator_node_deconstructor(Ast_operator_node_type *node, va_list arg_list)
{
	free(node->operator);
	array_list_destroy(node->children, Ast_node_deconstructor, NULL);

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



char *
Ast_node_debug_str(Ast_node_type *node, va_list arg_list)
{
	// string_buffer debug_str = string_buffer_create();

	// string_buffer_append(&debug_str, node->operator->desc);
	// string_buffer_append(&debug_str, ": ");

	// int operand_index;
	// for (operand_index = 0; operand_index < node->operands->length; operand_index ++)
	// {
	// 	if (operand_index == 0)
	// 	{
	// 		string_buffer_append(&debug_str, "{ ");
	// 	}
	// 	Ast_operand_type *operand = array_list_get(node->operand, operand_index);

	// }
	// return debug_str;
}





