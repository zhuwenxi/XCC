#include "LR_automata.h"
#include "LR_automata_test.h"
#include "context_free_grammar.h"
#include "logger.h"
#include "Ast.h"
#include "stack.h"


#include <string.h>
#include <assert.h>

typedef enum 
{
#define PRODUCTION_TOKEN(code, name) code,
#include "LR_automata_symbol.def"
#include "regexp_grammar.def"
#include "expression_grammar.def"
#undef PRODUCTION_TOKEN
	TOKEN_LIMIT
} token_type;

static char *token_desc_table[] =
{
#define PRODUCTION_TOKEN(code, name) name,
#include "LR_automata_symbol.def"
#include "regexp_grammar.def"
#include "expression_grammar.def"
#undef PRODUCTION_TOKEN
	"PRODUCTION_TOKEN_LIMIT"
};

static production_token_type 
get_token_type(char c)
{
	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
	{
		return ID;
	}
	else if (c == '*')
	{
		return MUL;
	}
	else if (c == '+')
	{
		return ADD;
	}
	else if (c == '(')
	{
		return LEFT_PARENTHESIS;
	}
	else if (c == ')')
	{
		return RIGHT_PARENTHESIS;
	}
	else
	{
		LOG(TRUE, "Oops, unknown token: %c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", c);
	}
}

bool
LR_automata_create_destroy_test()
{
	// context-free grammar for regexp
	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);

	context_free_grammar_add(cfg, REGEXP, REGEXP, VERTICAL_BAR, CONCAT, 0);
	context_free_grammar_add(cfg, REGEXP, CONCAT, 0);

	context_free_grammar_add(cfg, CONCAT, CONCAT, REPEAT, 0);
	context_free_grammar_add(cfg, CONCAT, REPEAT, 0);

	context_free_grammar_add(cfg, REPEAT, UNIT, STAR, 0);
	context_free_grammar_add(cfg, REPEAT, UNIT, 0);

	context_free_grammar_add(cfg, UNIT, LEFT_PARENTHESIS, CHAR, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, UNIT, CHAR, 0);

	// create LR automata
	LR_automata_type *lr_automata = LR_automata_create(cfg);

	return TRUE;
}

bool
LR_automata_expression_grammar_test()
{
	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);

	context_free_grammar_add(cfg, E, E, ADD, T, 0);
	context_free_grammar_add(cfg, E, T, 0);
	context_free_grammar_add(cfg, E, T, MUL, F, 0);
	context_free_grammar_add(cfg, T, F, 0);
	context_free_grammar_add(cfg, F, LEFT_PARENTHESIS, E, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, F, ID, 0);

	LR_automata_type *lr_automata = LR_automata_create(cfg);

	return TRUE;
}

bool
LR_automata_expression_grammar_right_recursive_test()
{
	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);

	context_free_grammar_add(cfg, E, T, E_SINGLE_QUOTATION, 0);
	context_free_grammar_add(cfg, E_SINGLE_QUOTATION, ADD, T, E_SINGLE_QUOTATION, 0);
	context_free_grammar_add(cfg, E_SINGLE_QUOTATION, EPSILON, 0);
	context_free_grammar_add(cfg, T, F, T_SINGLE_QUOTATION, 0);
	context_free_grammar_add(cfg, T_SINGLE_QUOTATION, MUL, F, T_SINGLE_QUOTATION, 0);
	context_free_grammar_add(cfg, T_SINGLE_QUOTATION, EPSILON, 0);
	context_free_grammar_add(cfg, F, LEFT_PARENTHESIS, E, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, F, ID, 0);

	LR_automata_type *lr_automata = LR_automata_create(cfg);

	return TRUE;
}

static void
construct_ast_for_expression_grammar(stack_type *ast_node_stack, production_type *prod_to_reduce)
{
	char *debug_str = production_debug_str(prod_to_reduce, token_desc_table);

	if (strcmp(debug_str, "F -> id Dot ") == 0)
	{
		LR_automata_input_type *stack_elem = stack_pop(ast_node_stack);
		assert(stack_elem);

		string_buffer desc = string_buffer_create();
		char tmp[2] = {stack_elem->c, '\0'};
		string_buffer_append(&desc, tmp);

		// create a operand node: "id"
		Ast_node_type *node = Ast_node_create(FALSE, desc, stack_elem->type);
		assert(node->is_operator_node == FALSE);
		stack_push(ast_node_stack, node);
	}
	else if (strcmp(debug_str, "E -> E + T Dot ") == 0)
	{
		Ast_node_type *node_T = stack_pop(ast_node_stack);

		// "+" symbol:
		Ast_node_type *node_add = stack_pop(ast_node_stack);

		Ast_node_type *node_E = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = {'+', '\0'};
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, ADD);
		Ast_append_sub_node(node, node_E);
		Ast_append_sub_node(node, node_T);

		stack_push(ast_node_stack, node);
	}
	else if (strcmp(debug_str, "T -> T * F Dot ") == 0)
	{
		Ast_node_type *node_F = stack_pop(ast_node_stack);

		// "*" symbol:
		Ast_node_type *node_star = stack_pop(ast_node_stack);

		Ast_node_type *node_T = stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[2] = {'*', '\0'};
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, MUL);
		Ast_append_sub_node(node, node_T);
		Ast_append_sub_node(node, node_F);

		stack_push(ast_node_stack, node);
	}
	else if (strcmp(debug_str, "T -> F Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else if (strcmp(debug_str, "F -> ( E ) Dot ") == 0)
	{
		// "(" symbol
		stack_pop(ast_node_stack);

		Ast_node_type *node_E = stack_pop(ast_node_stack);

		// ")" symbol
		stack_pop(ast_node_stack);

		string_buffer desc = string_buffer_create();
		char tmp[3] = {'(', ')', '\0'};
		string_buffer_append(&desc, tmp);

		Ast_node_type *node = Ast_node_create(TRUE, desc, LEFT_PARENTHESIS);
		Ast_append_sub_node(node, node_E);

		stack_push(ast_node_stack, node);
	}
	else if (strcmp(debug_str, "E -> T Dot ") == 0)
	{
		// Nothing to do. :)
	}
	else
	{
		LOG(TRUE, "Unknown production:%s!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!, len: %d", debug_str, strlen(debug_str));
	}

	free(debug_str);
}

bool
LR_automata_parse_test()
{
	//
	// Initialize LR automata:
	//

	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);

	context_free_grammar_add(cfg, E, E, ADD, T, 0);
	context_free_grammar_add(cfg, E, T, 0);
	context_free_grammar_add(cfg, T, T, MUL, F, 0);
	context_free_grammar_add(cfg, T, F, 0);
	context_free_grammar_add(cfg, F, LEFT_PARENTHESIS, E, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, F, ID, 0);

	LR_automata_type *lr_automata = LR_automata_create(cfg);



	//
	// Prepare input buffer:
	//

	LR_automata_input_buffer_type *buffer = LR_automata_input_buffer_create();
	LR_automata_input_buffer_init(buffer, "a*(b+c)", get_token_type);

	//
	// Parsing:
	//
	LR_automata_parse(lr_automata, buffer, construct_ast_for_expression_grammar);

	//
	// Prepare input buffer2:
	//

	LR_automata_input_buffer_type *buffer2 = LR_automata_input_buffer_create();
	LR_automata_input_buffer_init(buffer2, "a+b*(c+d)", get_token_type);

	//
	// Parsing:
	//
	LR_automata_parse(lr_automata, buffer2, construct_ast_for_expression_grammar);

	return TRUE;
}