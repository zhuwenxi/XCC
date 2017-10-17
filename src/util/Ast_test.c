#include "Ast_test.h"
#include "util/logger.h"

bool
Ast_create_destroy_test()
{
	Ast_type *ast = Ast_create();

	Ast_node_type *node = Ast_node_create(FALSE, "a", 1);
	ast->root = node;
	
	Ast_deconstructor(ast, NULL);

	return TRUE;
}

bool
Ast_construction_test()
{
	Ast_type *ast = Ast_create();

	Ast_node_type *node = Ast_node_create(TRUE, "*", 0);
	ast->root = node;

	Ast_node_type *sub_node1 = Ast_node_create(FALSE, "a", 0);
	Ast_append_sub_node(node, sub_node1);

	Ast_node_type *sub_node2 = Ast_node_create(TRUE, "+", 0);
	Ast_append_sub_node(node, sub_node2);

	Ast_node_type *sub_node3 = Ast_node_create(FALSE, "b", 0);
	Ast_node_type *sub_node4 = Ast_node_create(FALSE, "c", 0);
	Ast_append_sub_node(sub_node2, sub_node3);
	Ast_append_sub_node(sub_node2, sub_node4);

	// LOG(TRUE, "ast: %s", get_Ast_debug_str(ast, NULL));

	Ast_deconstructor(ast, NULL);

	return TRUE;
}
