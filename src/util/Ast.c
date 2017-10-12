#include "Ast.h"

#include <stdlib.h>


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
	return TRUE;
}

