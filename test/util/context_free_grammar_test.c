#include "context_free_grammar_test.h"
#include "context_free_grammar.h"
#include "stddefs.h"
#include "logger.h"
#include "util.h"

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

	context_free_grammar_add(cfg, 1, 2, 3, 0);
	context_free_grammar_add(cfg, 1, 2, 3, 0);
	context_free_grammar_add(cfg, 1, 2, 3, 0);

	context_free_grammar_destroy(cfg, NULL);
	return TRUE;
}