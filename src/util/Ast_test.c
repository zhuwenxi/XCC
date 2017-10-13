#include "Ast_test.h"

bool
Ast_create_destroy_test()
{
	Ast_type *ast = Ast_create();

	Ast_node_type *node = Ast_node_create();
	ast->root = node;
	
	Ast_deconstructor(ast, NULL);

	return TRUE;
}
