#include "LR_automata.h"
#include "LR_automata_test.h"

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
	"GOAL",
#define PRODUCTION_TOKEN(code, name) name,
#include "regexp_grammar.def"
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
}