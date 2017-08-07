#include "LR_automata.h"
#include "array_list.h"
#include "context_free_grammar.h"
#include "util.h"
#include "opts.h"
#include "logger.h"

#include <stdlib.h>
#include <assert.h>

typedef enum
{
#define PRODUCTION_TOKEN(code, name) code,
#include "LR_automata_symbol.def"
#undef PRODUCTION_TOKEN
} production_token_id;

void
construct_canonical_collection(LR_automata_type *lr_automata, context_free_grammar_type *grammar)
{
	//
	// Preprocess the gramamr, by adding an extra production "Goal -> [target production]", which represents the initial state of the parser.
	// In the regexp case, it is "Goal -> Regexp".
	//

	production_type *initial_production = production_create();
	*(initial_production->head) = GOAL;

	// retrieve the first production of the grammar
	// in the regexp case, it is "Regexp -> Regexp | Concat"
	assert(grammar->productions != NULL && grammar->productions->head != NULL);
	production_type *first_production = grammar->productions->head->data;

	// set first production head as body of initial production
	// in the regexp case, it is "Regexp"
	production_token_type first_production_head = *TYPE_CAST(first_production->head, int *);
	production_token_type *initial_production_body = create_int(first_production_head);
	linked_list_insert_back(initial_production->body, initial_production_body);

	LOG(LR_AUTOMATA_LOG_ENABLE, "initial production: %s", production_debug_str(initial_production, grammar->desc_table));

	// add the generated initial production to the given context-free grammar, as the new "first productoin".
	// in the regexp case, it is adding the production "Goal -> Regexp" to the first place of grammar
	linked_list_node_type *intial_production_node = linked_list_node_create();
	intial_production_node->data = initial_production;
	linked_list_insert_before(grammar->productions, grammar->productions->head, intial_production_node);
	
	LOG(LR_AUTOMATA_LOG_ENABLE, "the context-free grammar pass to LR automata: \n%s", get_context_free_grammar_debug_str(grammar));


	//
	// Construct the canonical collection of LR items
	//

	// the initial set "cc0"
	context_free_grammar_type *cc0 = context_free_grammar_copy(grammar, NULL);
	LOG(LR_AUTOMATA_LOG_ENABLE, "cc0: %s", get_context_free_grammar_debug_str(cc0));
	// array_list_append();
}

LR_automata_type *
LR_automata_create(context_free_grammar_type *grammar)
{
	LR_automata_type *lr_automata = (LR_automata_type *)malloc(sizeof(LR_automata_type));
	lr_automata->items = array_list_create();
	
	construct_canonical_collection(lr_automata, grammar);

	return lr_automata;
}

bool 
LR_automata_destory(LR_automata_type *lr_automata, ...)
{
	if (lr_automata == NULL)
	{
		return FALSE;
	}

	va_list ap;
	va_start(ap, lr_automata);
	LR_automata_deconstructor(lr_automata, ap);
	va_end(ap);

	return TRUE;
}

bool
LR_automata_deconstructor(LR_automata_type *lr_automata, va_list arg_list)
{
	if (lr_automata == NULL)
	{
		return FALSE;
	}

	array_list_deconstructor(lr_automata->items, arg_list);

	free(lr_automata);

	return TRUE;
}