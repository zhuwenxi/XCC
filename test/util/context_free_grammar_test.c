#include "context_free_grammar_test.h"
#include "context_free_grammar.h"
#include "stddefs.h"
#include "logger.h"
#include "util.h"

typedef enum 
{
#define PRODUCTION_TOKEN(code, name) code,
#include "LR_automata_symbol.def"
#include "regexp_grammar.def"
#undef PRODUCTION_TOKEN
} token_type;

static char *token_desc_table[] =
{
#define PRODUCTION_TOKEN(code, name) name,
#include "LR_automata_symbol.def"
#include "regexp_grammar.def"
#undef PRODUCTION_TOKEN
};

bool
production_create_destroy_test()
{
	production_type *production = production_create();

	production_destroy(production);

	return TRUE;
}

bool
context_free_grammar_create_destroy_test()
{
	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);
	context_free_grammar_destroy(cfg, NULL);
	return TRUE;
}

bool
context_free_grammar_add_production_test()
{
	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);

	context_free_grammar_add(cfg, REGEXP, REGEXP, VERTICAL_BAR, CONCAT, 0);
	context_free_grammar_add(cfg, REGEXP, CONCAT, 0);

	context_free_grammar_add(cfg, CONCAT, CONCAT, REPEAT, 0);
	context_free_grammar_add(cfg, CONCAT, REPEAT, 0);

	context_free_grammar_add(cfg, REPEAT, UNIT, STAR, 0);
	context_free_grammar_add(cfg, REPEAT, UNIT, 0);

	context_free_grammar_add(cfg, UNIT, LEFT_PARENTHESIS, CHAR, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, UNIT, CHAR, 0);

	context_free_grammar_destroy(cfg, NULL);
	return TRUE;
}

bool
context_free_grammar_debug_str_test()
{
	context_free_grammar_type *cfg = context_free_grammar_create(token_desc_table);

	context_free_grammar_add(cfg, REGEXP, REGEXP, VERTICAL_BAR, CONCAT, 0);
	context_free_grammar_add(cfg, REGEXP, CONCAT, 0);

	context_free_grammar_add(cfg, CONCAT, CONCAT, REPEAT, 0);
	context_free_grammar_add(cfg, CONCAT, REPEAT, 0);

	context_free_grammar_add(cfg, REPEAT, UNIT, STAR, 0);
	context_free_grammar_add(cfg, REPEAT, UNIT, 0);

	context_free_grammar_add(cfg, UNIT, LEFT_PARENTHESIS, REGEXP, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, UNIT, CHAR, 0);

	char *debug_str = get_context_free_grammar_debug_str(cfg);
	// LOG(TRUE, "%s", debug_str);
	free(debug_str);

	context_free_grammar_destroy(cfg, NULL);

	return TRUE;
}

bool
production_compare_test()
{
	production_type *prod1 = production_create();
	prod1->head = create_int(0);

	linked_list_insert_back(prod1->body, create_int(1));
	linked_list_insert_back(prod1->body, create_int(2));

	production_type *prod2 = production_create();
	prod2->head = create_int(0);

	linked_list_insert_back(prod2->body, create_int(1));
	linked_list_insert_back(prod2->body, create_int(2));

	if (production_compare(prod1, prod2) == FALSE) return FALSE;

	production_destroy(prod1, NULL);
	production_destroy(prod2, NULL);

	return TRUE;
}