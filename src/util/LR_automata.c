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

linked_list_type *
search_production_by_head(context_free_grammar_type *grammar, production_token_type *head)
{
	assert(grammar && grammar->productions && head);

	linked_list_node_type *production_node = grammar->productions->head;
	linked_list_type *ret_list = linked_list_create();

	while (production_node != NULL)
	{
		production_type *prod = (production_type *)production_node->data;
		production_token_type *prod_head = prod->head;

		if (*TYPE_CAST(prod_head, int *) == *TYPE_CAST(head, int *))
		{
			linked_list_insert_back(ret_list, prod);
		}

		production_node = production_node->next;
	}

	if (ret_list == NULL)
	{
		linked_list_destroy(ret_list, NULL);
		ret_list = NULL;
	}

	return ret_list;
}

context_free_grammar_type *
LR_automata_closure(context_free_grammar_type *state, context_free_grammar_type* grammar)
{
	assert(state && state->productions);
	
	linked_list_node_type *tail_node = NULL;

	while (state->productions->tail != tail_node)
	{	
		// for each item in "state"
		linked_list_node_type *item_node = state->productions->head;

		while (item_node != NULL)
		{	
			production_type *prod = (production_type *)item_node->data;
			int dot_value = DOT;
			linked_list_node_type *dot = linked_list_search(prod->body, &dot_value, int_comparator, NULL);

			if (dot != NULL && dot->next != NULL)
			{
				production_token_type *symbol_next_to_dot = dot->next->data;

				if (symbol_next_to_dot)
				{
					linked_list_type *searched_productions = search_production_by_head(grammar, symbol_next_to_dot);
					if (searched_productions != NULL)
					{	
						linked_list_node_type *searched_production = searched_productions->head;

						while (searched_production != NULL)
						{
							LOG(TRUE, "search the production: %s", production_debug_str(searched_production->data, grammar->desc_table));

							searched_production = searched_production->next;
						}
						
					}
				}
			}

			item_node = item_node->next;
		}

		tail_node = state->productions->tail;
	}

	return state;
}

context_free_grammar_type *
LR_automata_goto(context_free_grammar_type *state, production_token_type *symbol)
{
	//
	// TO DO
	//
	return state;
}

void
construct_canonical_collection(LR_automata_type *lr_automata, context_free_grammar_type *grammar)
{
	//
	// Preprocess the gramamr, by adding an extra production "Goal -> [target production]", which represents the initial state of the parser.
	// In the regexp case, it is "Goal -> Regexp".
	//
	production_type *initial_production = production_create();
	initial_production->head = create_int(GOAL);

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

	// the canonical collection "cc"
	array_list_type *cc = array_list_create();

	// the initial set "cc0"
	context_free_grammar_type *cc0 = context_free_grammar_create(grammar->desc_table);

	// add "Goal -> Dot Regexp" to "cc0"
	production_type *goal_production = production_copy(grammar->productions->head->data, NULL);
	linked_list_node_type *dot_node = linked_list_node_create();
	dot_node->data = create_int(DOT);
	linked_list_insert_before(goal_production->body, goal_production->body->head, dot_node);
	linked_list_insert_back(cc0->productions, goal_production);
	LOG(LR_AUTOMATA_LOG_ENABLE, "cc0: %s", get_context_free_grammar_debug_str(cc0));

	// add "cc0" to "cc"
	cc0 = LR_automata_closure(cc0, grammar);
	array_list_append(cc, cc0);

	int last_iter_cc_size = 0;

	// while new sets are still being added to "cc"
	while (cc->length > last_iter_cc_size)
	{	
		// for each unprocessed set
		int i;
		for (i = last_iter_cc_size; i < cc->length; i ++)
		{
			context_free_grammar_type *set = array_list_get(cc, i);

			// for each symbol following a "DOT" in an item in set
			linked_list_node_type *prod_node = set->productions->head;
			while (prod_node != NULL)
			{
				// search "DOT" in the production's body
				production_type *prod = prod_node->data;
				production_token_type *dot = create_int(DOT);
				linked_list_node_type *dot_node = linked_list_search(prod->body, dot, int_equal, NULL);
				
				if (dot_node != NULL)
				{
					production_token_type *next_symbol = dot_node->next->data;

					context_free_grammar_type *new_set = LR_automata_goto(set, next_symbol);

					// TO DO: if "new_set" doesn't exist in "cc", then add it to "cc"
					if (!array_list_search(cc, new_set, context_free_grammar_comparator, NULL))
					{
						array_list_append(cc, new_set);
					}
				}

				prod_node = prod_node->next;
			}
		}

		last_iter_cc_size = cc->length;
	}


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