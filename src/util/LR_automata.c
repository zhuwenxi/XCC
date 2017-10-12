#include "LR_automata.h"
#include "array_list.h"
#include "context_free_grammar.h"
#include "util.h"
#include "opts.h"
#include "logger.h"
#include "string_buffer.h"

#include <stdlib.h>
#include <assert.h>

static char **desc_table;

typedef enum
{
#define PRODUCTION_TOKEN(code, name) code,
#include "LR_automata_symbol.def"
#undef PRODUCTION_TOKEN
} production_token_id;

bool
is_terminal_symbol(LR_automata_type *lr_automata, production_token_type *symbol)
{
	int epsilon = EPSILON;
	return !linked_list_search(lr_automata->non_terminal_symbols, symbol, int_comparator, NULL) && int_comparator(symbol, &epsilon, NULL) == FALSE;
}

int
key_pair_hash(void *kp)
{
	assert(kp != NULL);

	lr_table_key_pair_type *key_pair = (lr_table_key_pair_type *)kp;
	int hashcode = 0;

	hashcode += *TYPE_CAST(key_pair->symbol, int *);

	linked_list_node_type *prod_node;
	for (prod_node = key_pair->state->productions->head; prod_node != NULL; prod_node = prod_node->next)
	{
		production_type *prod = TYPE_CAST(prod_node->data, production_type *);
		hashcode += *TYPE_CAST(prod->head, int *);

		linked_list_node_type *prod_body_node = prod->body->head;
		for (prod_body_node = prod->body->head; prod_body_node != NULL; prod_body_node = prod_body_node->next)
		{
			hashcode += *TYPE_CAST(prod_body_node->data, int *);
		}
	}

	return hashcode;
}

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

/*
 * Get all grammar symbol from the grammar, except for the "epsilon" symbol.
 */
linked_list_type *
get_all_grammar_symbol(LR_automata_type *lr_automata)
{	
	int epsilon = EPSILON;
	assert(lr_automata != NULL);
	context_free_grammar_type *grammar = lr_automata->grammar;
	assert(grammar != NULL);

	linked_list_type *symbols = linked_list_create();
	linked_list_node_type *prod_node = NULL;

	for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
	{
		production_type *prod = prod_node->data;

		assert(prod != NULL);

		production_token_type *head = prod->head;
		if (linked_list_search(symbols, head, int_comparator, NULL) == NULL && int_comparator(head, &epsilon, NULL) == FALSE)
		{
			linked_list_insert_back(symbols, head);
		}

		// update non_terminal_symbols:
		if (linked_list_search(lr_automata->non_terminal_symbols, head, int_comparator) == NULL)
		{
			linked_list_insert_back(lr_automata->non_terminal_symbols, head);
		}

		linked_list_type *body = prod->body;
		linked_list_node_type *body_token_node = NULL;
		for (body_token_node = body->head; body_token_node != NULL; body_token_node = body_token_node->next)
		{
			production_token_type *body_token = body_token_node->data;
			assert(body_token != NULL);

			if (linked_list_search(symbols, body_token, int_comparator, NULL) == NULL && int_comparator(body_token, &epsilon, NULL) == FALSE)
			{
				linked_list_insert_back(symbols, body_token);
			}
		}
	}

	return symbols;
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
LR_automata_transfer(context_free_grammar_type *state, production_token_type *symbol, LR_automata_type *lr_automata)
{
	context_free_grammar_type* grammar = lr_automata->grammar;

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
	else
	{
		new_state = LR_automata_closure(new_state, grammar);

		// update goto table
		lr_table_key_pair_type *key_pair = (lr_table_key_pair_type *)malloc(sizeof(lr_table_key_pair_type));
		key_pair->state = state;
		key_pair->symbol = symbol;

		hash_table_insert(lr_automata->goto_table, key_pair, new_state);
	}

	
	return new_state;
}

void
construct_canonical_collection(LR_automata_type *lr_automata)
{	
	context_free_grammar_type *grammar = lr_automata->grammar;
	//
	// collect grammar symbols
	//
	linked_list_type *grammar_symbols = get_all_grammar_symbol(lr_automata);
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

	lr_automata->first_set = LR_automata_construct_first_set(grammar);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "FIRST(): %s", get_set_debug_str(lr_automata->first_set, grammar->desc_table));

	lr_automata->follow_set = LR_automata_construct_follow_set(lr_automata->first_set, grammar);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "FOLLOW(): %s", get_set_debug_str(lr_automata->follow_set, grammar->desc_table));

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

	linked_list_node_type *non_terminal_symbol_node;
	string_buffer non_terminal_symbols_str = string_buffer_create();
	for (non_terminal_symbol_node = lr_automata->non_terminal_symbols->head; non_terminal_symbol_node != NULL; non_terminal_symbol_node = non_terminal_symbol_node->next)
	{
		production_token_type *symbol = non_terminal_symbol_node->data;
		string_buffer_append(&non_terminal_symbols_str, grammar->desc_table[*TYPE_CAST(symbol, int *)]);
		string_buffer_append(&non_terminal_symbols_str, " ");
	}
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "non_terminal_symbols: %s", non_terminal_symbols_str);

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
				context_free_grammar_type *next_state = LR_automata_transfer(set, grammar_symbol, lr_automata);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE && LR_AUTOMATA_GOTO_TABLE_LOG_ENABLE, "state:\n%s", get_context_free_grammar_debug_str(set));
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE && LR_AUTOMATA_GOTO_TABLE_LOG_ENABLE, "symbol: %s", grammar->desc_table[*TYPE_CAST(grammar_symbol, int *)]);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE && LR_AUTOMATA_GOTO_TABLE_LOG_ENABLE, "next_state:\n%s", get_context_free_grammar_debug_str(next_state));
				
				if (next_state != NULL && array_list_search(cc, next_state, context_free_grammar_comparator, NULL) == NULL)
				{
					array_list_append(cc, next_state);
				}
			}
		}

		last_iter_cc_size = cc->length;
	}

	lr_automata->items = cc;

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "state count: %d, cc: \n%s\n", cc->length, get_array_list_debug_str(cc, context_free_grammar_debug_str, NULL));
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "GOTO TABLE: \n%s\n", get_hash_table_debug_str(lr_automata->goto_table, lr_table_key_pair_debug_str, context_free_grammar_debug_str, NULL));
}

void
construct_action_table(LR_automata_type *lr_automata)
{
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "=========================================== start to construct ACTION table ===========================================");
	array_list_type *states = lr_automata->items;
	int i;
	for (i = 0; i < states->length; i ++)
	{
		context_free_grammar_type *state = array_list_get(states, i);

		linked_list_node_type *prod_node;
		for (prod_node = state->productions->head; prod_node != NULL; prod_node = prod_node->next)
		{
			production_type *prod = prod_node->data;

			int dot = DOT;
			linked_list_node_type *dot_node = linked_list_search(prod->body, &dot, int_comparator, NULL);
			if (dot_node != NULL)
			{	
				if (dot_node->next != NULL)
				{
					// if [A-> alpha DOT a beta] is in Ii and GOTO[Ii, a] = Ij, then set ACTION[i, a] to "shift j", here a is a terminal
					production_token_type *symbol_next_to_dot = dot_node->next->data;

					if (is_terminal_symbol(lr_automata, symbol_next_to_dot))
					{
						lr_table_key_pair_type *key = (lr_table_key_pair_type *)malloc(sizeof(lr_table_key_pair_type));
						key->state = state;
						key->symbol = symbol_next_to_dot;

						LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "SHIFT, state: %s, symbol: %s", get_context_free_grammar_debug_str(key->state), state->desc_table[*TYPE_CAST(key->symbol, int *)]);

						action_table_value *value = (action_table_value *)malloc(sizeof(action_table_value));
						value->action = SHIFT;
						value->next_state = hash_table_search(lr_automata->goto_table, key, lr_table_key_pair_comparator, NULL);
						assert(value->next_state);
						value->prod_to_reduce = NULL;

						hash_table_insert(lr_automata->action_table, key, value);

						LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "SHIFT done");

					}
				}
				else
				{
					
					int goal_symbol = GOAL;
					if (int_comparator(prod->head, &goal_symbol, NULL))
					{
						// if [Goal -> A DOT] is in Ii, then set ACTION[i, $] to "accept"
						lr_table_key_pair_type *key = (lr_table_key_pair_type *)malloc(sizeof(lr_table_key_pair_type));
						key->state = state;
						production_token_type *dollar_symbol = create_int(DOLLAR);
						key->symbol = dollar_symbol;

						action_table_value *value = (action_table_value *)malloc(sizeof(action_table_value));
						value->action = ACCEPT;
						value->next_state = NULL;
						value->prod_to_reduce = NULL;

						hash_table_insert(lr_automata->action_table, key, value);
					}
					else
					{
						// if [A -> alpah DOT] is in Ii, then set ACTION[i, a] to "reduce A -> Alpha" for all a in FOLLOW(A), here A may not be Goal
						linked_list_type *symbols_follow_A = LR_automata_follow(lr_automata->follow_set, prod->head);
						assert(symbols_follow_A != NULL && symbols_follow_A->head != NULL);

						linked_list_node_type *symbol_node;
						for (symbol_node = symbols_follow_A->head; symbol_node != NULL; symbol_node = symbol_node->next)
						{
							lr_table_key_pair_type *key = (lr_table_key_pair_type *)malloc(sizeof(lr_table_key_pair_type));
							key->state = state;
							key->symbol = symbol_node->data;

							action_table_value *value = (action_table_value *)malloc(sizeof(action_table_value));
							value->action = REDUCE;
							value->next_state = NULL;
							value->prod_to_reduce = production_copy(prod, NULL);

							hash_table_insert(lr_automata->action_table, key, value);
						}
						

					}
				}
			} 

			


		}
	}

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "============================================ ACTION table constructed ===========================================");
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "ACTION TABLE:\n%s", get_hash_table_debug_str(lr_automata->action_table, lr_table_key_pair_debug_str, action_table_value_debug_str, NULL));
}

LR_automata_type *
LR_automata_create(context_free_grammar_type *grammar)
{
	LR_automata_type *lr_automata = (LR_automata_type *)malloc(sizeof(LR_automata_type));
	lr_automata->items = array_list_create();
	lr_automata->grammar = grammar;
	lr_automata->non_terminal_symbols = linked_list_create();

	// Initialize goto & action tables
	lr_automata->goto_table = hash_table_create(key_pair_hash);
	lr_automata->action_table = hash_table_create(key_pair_hash);
	desc_table = grammar->desc_table;

	// Initialize first & follow sets
	lr_automata->first_set = NULL; 
	lr_automata->follow_set = NULL;

	construct_canonical_collection(lr_automata);
	construct_action_table(lr_automata);

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

void
LR_automata_parse(LR_automata_type *lr_automata, char *text)
{
	
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

char *lr_table_key_pair_debug_str(lr_table_key_pair_type *key_pair, va_list arg_list)
{
	assert(key_pair != NULL);

	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, context_free_grammar_debug_str(key_pair->state, NULL));
	string_buffer_append(&debug_str, " , ");
	string_buffer_append(&debug_str, desc_table[*TYPE_CAST(key_pair->symbol ,int *)]); 

	return debug_str;
}

bool
lr_table_key_pair_comparator(void *k1, void *k2, va_list arg_list)
{
	lr_table_key_pair_type *key1 = TYPE_CAST(k1, lr_table_key_pair_type *);
	lr_table_key_pair_type *key2 = TYPE_CAST(k2, lr_table_key_pair_type *);

	if (key1 == key2) return TRUE;
	if (key1 == NULL || key2 == NULL)
	{
		if (key1 == NULL && key2 == NULL)
			return TRUE;
		else
			return FALSE;
	}

	
	assert(key1->state && key2->state && key1->symbol && key2->symbol);
	return context_free_grammar_comparator(key1->state, key2->state, NULL) && int_comparator(key1->symbol, key2->symbol, NULL);
}

char *
action_table_value_debug_str(action_table_value *value, va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	if (value->action == ACCEPT)
	{
		string_buffer_append(&debug_str, "Accept ");
		string_buffer_append(&debug_str, "\n\n");
	}
	else if (value->action == SHIFT)
	{
		string_buffer_append(&debug_str, "Shift to ");
		string_buffer_append(&debug_str, get_context_free_grammar_debug_str(value->next_state));
		string_buffer_append(&debug_str, "\n\n");
	}
	else if (value->action == REDUCE)
	{
		string_buffer_append(&debug_str, "Reduce to ");
		string_buffer_append(&debug_str, production_debug_str(value->prod_to_reduce, desc_table));
		string_buffer_append(&debug_str, "\n\n");
	}

	return debug_str;
}

linked_list_type *
LR_automata_follow(array_list_type *follow_set, production_token_type *symbol)
{	
	assert(follow_set && symbol);
	return array_list_get(follow_set, *TYPE_CAST(symbol, int *));
}

array_list_type *
LR_automata_construct_follow_set(array_list_type *first_set, context_free_grammar_type *grammar)
{
	array_list_type *follow_set = array_list_create();
	bool follow_set_has_changed = FALSE;

	// collect all non-terminal symbols
	linked_list_type *non_terminal_symbols = linked_list_create();
	linked_list_node_type *prod_node = NULL;
	for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
	{
		production_type *prod = prod_node->data;
		if (linked_list_search(non_terminal_symbols, prod->head, int_comparator) == NULL)
		{
			linked_list_insert_back(non_terminal_symbols, prod->head);
		}
	}

	// Place $ in FOLLOW(GOAL)
	int goal_symbol = GOAL;
	int *dollar_symbol = create_int(DOLLAR);

	linked_list_type *update_set = linked_list_create();
	linked_list_insert_back(update_set, dollar_symbol);

	LR_automata_set_update(follow_set, &goal_symbol, update_set);
	

	linked_list_destroy(update_set, NULL);

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "initial FOLLOW set: %s", get_set_debug_str(follow_set, grammar->desc_table));

	do {
		
		linked_list_node_type *prod_node = NULL;
		for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
		{
			production_type *prod = prod_node->data;
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "process %s", production_debug_str(prod, grammar->desc_table));

			linked_list_type *trailer = linked_list_create();
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "FOLLOW(%s): %s", grammar->desc_table[*TYPE_CAST(prod->head, int *)], get_sub_set_debug_str(LR_automata_follow(follow_set, prod->head), grammar->desc_table));
			linked_list_merge(trailer, LR_automata_follow(follow_set, prod->head), int_comparator, int_copier, NULL);

			linked_list_node_type *prod_token_node;
			for (prod_token_node = prod->body->tail; prod_token_node != NULL; prod_token_node = prod_token_node->prev)
			{
				production_token_type *prod_token = prod_token_node->data;

				if (linked_list_search(non_terminal_symbols, prod_token, int_comparator, NULL) != NULL)
				{
					bool has_changed = LR_automata_set_update(follow_set, prod_token, trailer);
					LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "update FOLLOW(%s): %s", grammar->desc_table[*TYPE_CAST(prod_token, int *)], get_set_debug_str(follow_set, grammar->desc_table));

					follow_set_has_changed = has_changed ? TRUE : has_changed;

					linked_list_type *first_set_of_prod_token = LR_automata_first(first_set, prod_token);
					production_token_type epsilon_symbol = EPSILON;

					LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "FIRST(%s): %s", grammar->desc_table[*TYPE_CAST(prod_token, int *)], get_sub_set_debug_str(first_set_of_prod_token, grammar->desc_table));
					// LOG(TRUE, "%s", get_sub_set_debug_str(first_set_of_prod_token, grammar->desc_table));
					if (linked_list_search(first_set_of_prod_token, &epsilon_symbol, int_comparator, NULL))
					{
						// add everthing but epsilon to trailer
						linked_list_type *first_set_but_epsilon = linked_list_create();
						linked_list_node_type *n;
						for (n = first_set_of_prod_token->head; n != NULL; n = n->next)
						{
							if (!int_comparator(n->data, &epsilon_symbol, NULL))
							{
								linked_list_insert_back(first_set_but_epsilon, int_copier(n->data, NULL));
							}
						}

						linked_list_merge(trailer, first_set_but_epsilon, int_comparator, int_copier, NULL);
						LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "merge, trailer: %s", get_sub_set_debug_str(trailer, grammar->desc_table));

					}
					else
					{
						trailer = linked_list_copy(first_set_of_prod_token, int_copier, NULL);
						LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "truncate, trailer: %s", get_sub_set_debug_str(trailer, grammar->desc_table));
					}

				}
				else
				{
					linked_list_destroy(trailer, NULL);
					trailer = linked_list_create();
					linked_list_merge(trailer, LR_automata_first(first_set, prod_token), int_comparator, int_copier, NULL);
					LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "non-terminal, trailer: %s", get_sub_set_debug_str(trailer, grammar->desc_table));
				}
			}

			linked_list_destroy(trailer, NULL);
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "process done ==============================================================================");

		}
	} while (follow_set_has_changed == TRUE);
	
	
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "follow set: \n  %s", get_set_debug_str(follow_set, grammar->desc_table));

	return follow_set;
}

linked_list_type *
LR_automata_first(array_list_type *first_set, production_token_type *symbol)
{
	assert(first_set && symbol);
	return array_list_get(first_set, *TYPE_CAST(symbol, int *));
}

array_list_type *
LR_automata_construct_first_set(context_free_grammar_type *grammar)
{	
	array_list_type *first_set = array_list_create();
	bool first_set_has_changed = FALSE;

	int epsilon = EPSILON;

	// collect all non-terminal symbols
	linked_list_type *non_terminal_symbols = linked_list_create();
	linked_list_node_type *prod_node = NULL;
	for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
	{
		production_type *prod = prod_node->data;
		if (linked_list_search(non_terminal_symbols, prod->head, int_comparator) == NULL)
		{
			linked_list_insert_back(non_terminal_symbols, prod->head);
		}
	}

	//
	// FIRST(alpha) <- alpha, if alpha is terminal symbol.
	//

	// Record symbols we already met before, to avoid redundant visits.
	linked_list_type *symbol_list = linked_list_create();
	for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
	{
		production_type *prod = prod_node->data;
		production_token_type *head = prod->head;
		if (linked_list_search(symbol_list, head, int_comparator, NULL) == NULL)
		{
			// record this symbol, mark it "visited".
			linked_list_insert_back(symbol_list, head);

			if (linked_list_search(non_terminal_symbols, head, int_comparator, NULL) == NULL)
			{
				// if it's a terminal symbol, FIRST(alpha) <- alpha
				linked_list_type *set_of_alpha = linked_list_create();
				linked_list_insert_back(set_of_alpha, head);
				array_list_set(first_set, *TYPE_CAST(head, int *), set_of_alpha);
			}
			else
			{
				// if it's a non-terminal symbol, nothing we should do but initialize it as a empty linked list.
				array_list_set(first_set, *TYPE_CAST(head, int *), linked_list_create());
			}
		}

		linked_list_node_type *body_node;
		for (body_node = prod->body->head; body_node != NULL; body_node = body_node->next)
		{
			production_token_type *body_token = body_node->data;
			if (linked_list_search(symbol_list, body_token, int_comparator, NULL) == NULL)
			{
				linked_list_insert_back(symbol_list, body_token);

				if (linked_list_search(non_terminal_symbols, body_token, int_comparator, NULL) == NULL)
				{
					linked_list_type *set_of_alpha = linked_list_create();
					linked_list_insert_back(set_of_alpha, body_token);
					array_list_set(first_set, *TYPE_CAST(body_token, int *), set_of_alpha);
				}
				else
				{
					array_list_set(first_set, *TYPE_CAST(body_token, int *), linked_list_create());
				}
			}
			
		}
	}
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "initialized first set:\n%s\n", get_set_debug_str(first_set, grammar->desc_table));

	linked_list_destroy(symbol_list, NULL);

	do {
		first_set_has_changed = FALSE;

		linked_list_node_type *prod_node;
		for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
		{
			production_type *prod = prod_node->data;
			assert(prod);

			linked_list_type *first_set_of_current_symbol = linked_list_create();

			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "from the production %s", production_debug_str(prod, grammar->desc_table));

			linked_list_node_type *body_node;
			for (body_node = prod->body->head; body_node != NULL; body_node = body_node->next)
			{

				linked_list_type *first_set_of_b1 = array_list_get(first_set, *TYPE_CAST(body_node->data, int *));
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "first set of %s is: %s", grammar->desc_table[*TYPE_CAST(body_node->data, int *)], get_sub_set_debug_str(first_set_of_b1, grammar->desc_table));

				linked_list_type *first_set_of_b1_without_epsilon = linked_list_create();

				linked_list_node_type *node;
				for (node = first_set_of_b1->head; node != NULL; node = node->next)
				{
					if (int_comparator(node->data, &epsilon, NULL) == FALSE) 
					{
						linked_list_insert_back(first_set_of_b1_without_epsilon, node->data);
					}
				}

				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "exclude epsilon: %s", get_sub_set_debug_str(first_set_of_b1_without_epsilon, grammar->desc_table));

				linked_list_merge(first_set_of_current_symbol, first_set_of_b1_without_epsilon, int_comparator, int_copier, NULL);

				if (linked_list_search(first_set_of_b1, &epsilon, int_comparator, NULL) == NULL) break;

				if (body_node == prod->body->tail && linked_list_search(first_set_of_b1, &epsilon, int_comparator, NULL))
				{
					linked_list_insert_back(first_set_of_current_symbol, create_int(epsilon));
				}
			}

			bool has_change = LR_automata_set_update(first_set, prod->head, first_set_of_current_symbol);
			if (has_change)
			{
				first_set_has_changed = TRUE;
			}
			
		}

		LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "first set, this round:\n%s\n", get_set_debug_str(first_set, grammar->desc_table));

	} while (first_set_has_changed);

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "first set:\n%s\n", get_set_debug_str(first_set, grammar->desc_table));

	return first_set;
}

bool
LR_automata_set_update(array_list_type *set, production_token_type *symbol, linked_list_type *update_set)
{
	int symbol_value = *TYPE_CAST(symbol, int *);

	// set is not large enough to hold the follow(symbol)
	if (set->length < symbol_value + 1)
	{
		array_list_adjust_length(set, symbol_value + 1);
	}

	linked_list_type *follow_set_of_symbol = array_list_get(set, symbol_value);

	// no item in follwow(symbol)
	if (follow_set_of_symbol == NULL)
	{
		follow_set_of_symbol = linked_list_create();
		array_list_set(set, symbol_value, follow_set_of_symbol);
	}

	return linked_list_merge(follow_set_of_symbol, update_set, int_comparator, int_copier, NULL);
}

static char*
get_set_debug_str(array_list_type *set, char *desc_table[])
{
	assert(set);

	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, "{ ");
	int i;
	for (i = 0; i < set->length; i ++)
	{
		linked_list_type *set_of_one_symbol = array_list_get(set, i);
		if (set_of_one_symbol == NULL) continue;

		string_buffer_append(&debug_str, desc_table[i]);
		string_buffer_append(&debug_str, ": ");

		linked_list_node_type *node;
		string_buffer_append(&debug_str, "[ ");
		for (node = set_of_one_symbol->head; node != NULL; node = node->next)
		{
			production_type *symbol = node->data;
			string_buffer_append(&debug_str, desc_table[*TYPE_CAST(symbol, int *)]);
			
			if (node != set_of_one_symbol->tail)
			{
				string_buffer_append(&debug_str, ", ");
			}

		}
		string_buffer_append(&debug_str, " ], ");
	}

	string_buffer_append(&debug_str, " }");

	return debug_str;
}

static char *
get_sub_set_debug_str(linked_list_type *set, char *desc_table[])
{
	string_buffer ret = string_buffer_create();
	string_buffer_append(&ret, "{ ");

	if (set == NULL)
	{
		string_buffer_append(&ret, " } ");
		return ret;
	}

	linked_list_node_type *node;
	for (node = set->head; node != NULL; node = node->next)
	{
		production_token_type *token = node->data;
		string_buffer_append(&ret, desc_table[*TYPE_CAST(token, int *)]);
		if (node != set->tail)
			string_buffer_append(&ret, ", ");
	}

	string_buffer_append(&ret, " } ");
	return ret;
}

