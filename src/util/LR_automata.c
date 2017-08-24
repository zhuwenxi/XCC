#include "LR_automata.h"
#include "array_list.h"
#include "context_free_grammar.h"
#include "util.h"
#include "opts.h"
#include "logger.h"
#include "string_buffer.h"

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

linked_list_type *
get_all_grammar_symbol(context_free_grammar_type *grammar)
{
	assert(grammar != NULL);

	linked_list_type *symbols = linked_list_create();
	linked_list_node_type *prod_node = NULL;

	for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
	{
		production_type *prod = prod_node->data;

		assert(prod != NULL);

		production_token_type *head = prod->head;
		if (linked_list_search(symbols, head, int_comparator, NULL) == NULL)
		{
			linked_list_insert_back(symbols, head);
		}

		linked_list_type *body = prod->body;
		linked_list_node_type *body_token_node = NULL;
		for (body_token_node = body->head; body_token_node != NULL; body_token_node = body_token_node->next)
		{
			production_token_type *body_token = body_token_node->data;
			assert(body_token != NULL);

			if (linked_list_search(symbols, body_token, int_comparator, NULL) == NULL)
			{
				linked_list_insert_back(symbols, body_token);
			}
		}
	}

	return symbols;
}

linked_list_type *
LR_automata_first(linked_list_type *symbols, context_free_grammar_type *grammar)
{
	return NULL;
}

context_free_grammar_type *
LR_automata_closure(context_free_grammar_type *state, context_free_grammar_type* grammar)
{
	if (state == NULL) return NULL;
	
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
						linked_list_node_type *searched_production_node = searched_productions->head;

						while (searched_production_node != NULL)
						{
							production_type *searched_production_copy = production_copy(searched_production_node->data, NULL);
							linked_list_node_type *inserted_dot_node = linked_list_node_create();
							inserted_dot_node->data = create_int(DOT);
							linked_list_insert_before(searched_production_copy->body, searched_production_copy->body->head, inserted_dot_node);

							if (linked_list_search(state->productions, searched_production_copy, production_comparator, NULL) == NULL)
								context_free_grammar_add_production(state, searched_production_copy);

							searched_production_node = searched_production_node->next;
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
LR_automata_goto(context_free_grammar_type *state, production_token_type *symbol, context_free_grammar_type* grammar)
{
	// for each symbol following a "DOT" in an item in set
	context_free_grammar_type *new_state = context_free_grammar_create(grammar->desc_table);
	linked_list_node_type *prod_node = state->productions->head;
	
	while (prod_node != NULL)
	{
		// search "DOT" in the production's body
		production_type *prod = prod_node->data;
		production_token_type *dot = create_int(DOT);
		linked_list_node_type *dot_node = linked_list_search(prod->body, dot, int_equal, NULL);
				
		if (dot_node != NULL && dot_node->next != NULL)
		{
			production_type *prod_copy = production_copy(prod, NULL);
			dot_node = linked_list_search(prod_copy->body, dot, int_equal, NULL);
			linked_list_node_type *next_node = dot_node->next;

			if (int_comparator(next_node->data, symbol, NULL))
			{
				linked_list_switch_node(prod_copy->body, dot_node, next_node);
				void *res = linked_list_search(new_state->productions, prod_copy, production_comparator, NULL);
				if (res == NULL)
				{
					context_free_grammar_add_production(new_state, prod_copy);
				}
				else
				{
					context_free_grammar_destroy(new_state, NULL);
				}
			}
		}

		prod_node = prod_node->next;
	}

	if (new_state->productions->head == NULL)
	{
		context_free_grammar_destroy(new_state, NULL);
		new_state = NULL;
	}

	return LR_automata_closure(new_state, grammar);
}

void
construct_canonical_collection(LR_automata_type *lr_automata, context_free_grammar_type *grammar)
{	
	//
	// collect grammar symbols
	//
	linked_list_type *grammar_symbols = get_all_grammar_symbol(grammar);

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

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "initial production: %s", production_debug_str(initial_production, grammar->desc_table));

	// add the generated initial production to the given context-free grammar, as the new "first productoin".
	// in the regexp case, it is adding the production "Goal -> Regexp" to the first place of grammar
	linked_list_node_type *intial_production_node = linked_list_node_create();
	intial_production_node->data = initial_production;
	linked_list_insert_before(grammar->productions, grammar->productions->head, intial_production_node);
	
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "the context-free grammar pass to LR automata: \n%s", get_context_free_grammar_debug_str(grammar));


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
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "cc0: %s", get_context_free_grammar_debug_str(cc0));

	// add "cc0" to "cc"
	cc0 = LR_automata_closure(cc0, grammar);
	array_list_append(cc, cc0);

	// get all grammar symbols
	linked_list_node_type *symbol_node = NULL;
	string_buffer symbol_str = string_buffer_create();
	for (symbol_node = grammar_symbols->head; symbol_node != NULL; symbol_node = symbol_node->next)
	{
		production_token_type *symbol = symbol_node->data;
		string_buffer_append(&symbol_str, grammar->desc_table[*TYPE_CAST(symbol, int *)]);
		string_buffer_append(&symbol_str, " ");
	}
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "symbols: %s", symbol_str);
	assert(grammar_symbols != NULL);

	int last_iter_cc_size = 0;

	// while new sets are still being added to "cc"
	while (cc->length > last_iter_cc_size)
	{	
		// for each unprocessed set
		int i;
		for (i = last_iter_cc_size; i < cc->length; i ++)
		{
			context_free_grammar_type *set = array_list_get(cc, i);

			// for each grammar symbol
			linked_list_node_type *grammar_symbol_node = NULL;

			for (grammar_symbol_node = grammar_symbols->head; grammar_symbol_node != NULL; grammar_symbol_node = grammar_symbol_node->next)
			{
				production_token_type *grammar_symbol = grammar_symbol_node->data;
				context_free_grammar_type *next_state = LR_automata_goto(set, grammar_symbol, grammar);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "state:\n%s", get_context_free_grammar_debug_str(set));
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "symbol: %s", grammar->desc_table[*TYPE_CAST(grammar_symbol, int *)]);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "next_state:\n%s", get_context_free_grammar_debug_str(next_state));
				
				if (next_state != NULL && array_list_search(cc, next_state, context_free_grammar_comparator, NULL) == NULL)
				{
					array_list_append(cc, next_state);
				}
			}
		}

		last_iter_cc_size = cc->length;
	}

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "cc->length: %d, cc: \n%s", cc->length, get_array_list_debug_str(cc, context_free_grammar_debug_str, NULL));

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