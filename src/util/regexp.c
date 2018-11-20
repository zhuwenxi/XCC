#include "regexp.h"



char *token_desc_table[] =
{
#define PRODUCTION_TOKEN(code, name) name,
#include "LR_automata_symbol.def"
#include "regexp_grammar.def"
#include "expression_grammar.def"
#undef PRODUCTION_TOKEN
	"PRODUCTION_TOKEN_LIMIT"
};

regexp_type *
regexp_create(char *text)
{
	regexp_type *self = (regexp_type *)malloc(sizeof(regexp_type));
	self->text = text;

	return self;
}

bool
regexp_deconstructor(regexp_type *regexp, va_list arg_list)
{
	bool dfa_destroyed = LR_automata_destroy(regexp->DFA);
	free(regexp);
	
	return dfa_destroyed;
}

static void
thompson_consturction()
{

}

char *regexp_search(char *pattern, char *str)
{
	
}

production_token_type
regexp_grammar_get_token_type(char c)
{
	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
	{
		return CHAR;
	}
	else if (c == '*')
	{
		return STAR;
	}
	else if (c == '(')
	{
		return LEFT_PARENTHESIS;
	}
	else if (c == ')')
	{
		return RIGHT_PARENTHESIS;
	}
	else if (c == '|')
	{
		return VERTICAL_BAR;
	}
	else
	{
		LOG(TRUE, "Oops, unknown token: %c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", c);
	}
}

void
construct_ast_for_regexp_grammar(stack_type *ast_node_stack, production_type *prod_to_reduce)
{
	char *debug_str = production_debug_str(prod_to_reduce, token_desc_table);

	if (strcmp(debug_str, "Regexp -> Regexp | Concat Dot ") == 0)
	{
		Ast_node_type *node_Concat = stack_pop(ast_node_stack);

		// "|" symbol:
		stack_pop(ast_node_stack);

		Ast_node_type *node_Regexp = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { '|', '\0' };
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, VERTICAL_BAR);
		Ast_append_sub_node(node, node_Regexp);
		Ast_append_sub_node(node, node_Concat);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Regexp -> Concat Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else if (strcmp(debug_str, "Concat -> Concat Repeat Dot ") == 0)
	{
		Ast_node_type *node_Repeat = stack_pop(ast_node_stack);
		Ast_node_type *node_Concat = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { '.', '\0' };
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, CONCAT);
		Ast_append_sub_node(node, node_Concat);
		Ast_append_sub_node(node, node_Repeat);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Concat -> Repeat Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else if (strcmp(debug_str, "Repeat -> Unit * Dot ") == 0)
	{
		// "*" symbol:
		stack_pop(ast_node_stack);

		Ast_node_type *node_Unit = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { '*', '\0' };
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, STAR);
		Ast_append_sub_node(node, node_Unit);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Repeat -> Unit Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else if (strcmp(debug_str, "Unit -> ( Regexp ) Dot ") == 0)
	{
		// "(" symbol:
		stack_pop(ast_node_stack);

		Ast_node_type *node_Regexp = stack_pop(ast_node_stack);

		// ")" symbol:
		stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		string_buffer_append(&desc, "()");

		Ast_node_type *node = Ast_node_create(TRUE, desc, LEFT_PARENTHESIS);
		Ast_append_sub_node(node, node_Regexp);

		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else if (strcmp(debug_str, "Unit -> Char Dot ") == 0)
	{
		LR_automata_input_type *stack_elem = stack_pop(ast_node_stack);
		assert(stack_elem);

		string_buffer desc = string_buffer_create();
		char tmp[2] = { stack_elem->c, '\0' };
		string_buffer_append(&desc, tmp);

		// create a operand node: "id"
		Ast_node_type *node = Ast_node_create(FALSE, desc, stack_elem->type);
		assert(node->is_operator_node == FALSE);
		stack_push(ast_node_stack, node);

		string_buffer_destroy(desc);
	}
	else
	{
		LOG(TRUE, "Unknown production:%s!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!, len: %d", debug_str, strlen(debug_str));
	}

	free(debug_str);
}
