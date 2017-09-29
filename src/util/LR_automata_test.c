#include "LR_automata.h"
#include "LR_automata_test.h"

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