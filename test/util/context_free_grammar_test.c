#include "context_free_grammar_test.h"
#include "context_free_grammar.h"
#include "stddefs.h"
#include "logger.h"
#include "util.h"

typedef enum 
{
	TOKEN_UNKNOWN,
#define PRODUCTION_TOKEN(code, name) code,
#include "regexp_grammar.def"
#undef PRODUCTION_TOKEN
	TOKEN_LIMIT
} token_type;

static char *token_desc_table[] =
{
	"UNKNOWN",
#define PRODUCTION_TOKEN(code, name) name,
#include "regexp_grammar.def"
#undef PRODUCTION_TOKEN
	"PRODUCTION_TOKEN_LIMIT"
};

bool
context_free_grammar_create_destroy_test()
{
	context_free_grammar_type *cfg = context_free_grammar_create();
	context_free_grammar_destroy(cfg, NULL);
	return TRUE;
}

bool
context_free_grammar_add_production_test()
{
	context_free_grammar_type *cfg = context_free_grammar_create();

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
	context_free_grammar_type *cfg = context_free_grammar_create();

	context_free_grammar_add(cfg, REGEXP, REGEXP, VERTICAL_BAR, CONCAT, 0);
	context_free_grammar_add(cfg, REGEXP, CONCAT, 0);

	context_free_grammar_add(cfg, CONCAT, CONCAT, REPEAT, 0);
	context_free_grammar_add(cfg, CONCAT, REPEAT, 0);

	context_free_grammar_add(cfg, REPEAT, UNIT, STAR, 0);
	context_free_grammar_add(cfg, REPEAT, UNIT, 0);

	context_free_grammar_add(cfg, UNIT, LEFT_PARENTHESIS, REGEXP, RIGHT_PARENTHESIS, 0);
	context_free_grammar_add(cfg, UNIT, CHAR, 0);

	char *debug_str = get_context_free_grammar_debug_str(cfg, token_desc_table);
	// LOG(TRUE, "%s", debug_str);
	free(debug_str);

	context_free_grammar_destroy(cfg, NULL);

	return TRUE;
}