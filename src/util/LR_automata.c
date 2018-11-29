#include "LR_automata.h"
#include "array_list.h"
#include "context_free_grammar.h"
#include "util.h"
#include "opts.h"
#include "logger.h"
#include "string_buffer.h"
#include "stack.h"

#include <stdlib.h>
#include <assert.h>

static char **desc_table;

typedef enum
{
#define PRODUCTION_TOKEN(code, name) code,
#include "LR_automata_symbol.def"
#undef PRODUCTION_TOKEN
} production_token_id;

production_token_type LR_automata_goal_symbol = GOAL;
production_token_type LR_automata_dollar_symbol = DOLLAR;
production_token_type LR_automata_epsilon_symbol = EPSILON;




bool
table_key_deconstructor(LR_table_key_pair_type *key1, va_list arg_list)
{
	free(key1);

	return TRUE;
}

bool
action_table_value_deconstructor(action_table_value_type *value, va_list arg_list)
{
	free(value);

	return TRUE;
}

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

	LR_table_key_pair_type *key_pair = (LR_table_key_pair_type *)kp;
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
							{
								context_free_grammar_add_production(state, searched_production_copy);
							}
							else
							{
								production_deconstructor(searched_production_copy, NULL);
							}

							searched_production_node = searched_production_node->next;

						}
						
					}

					linked_list_destroy(searched_productions, NULL);
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
	LR_table_key_pair_type *key_pair = (LR_table_key_pair_type *)malloc(sizeof(LR_table_key_pair_type));
	key_pair->state = state;
	key_pair->symbol = symbol;

	// LOG(TRUE, "state: %s, symbol: %s", context_free_grammar_debug_str(state, NULL), lr_automata->grammar->desc_table[*TYPE_CAST(symbol, int *)]);

	context_free_grammar_type *new_state = hash_table_search(lr_automata->goto_table, key_pair, lr_table_key_pair_comparator, NULL);

	if (new_state == NULL)
	{
		context_free_grammar_type* grammar = lr_automata->grammar;

		// for each symbol following a "DOT" in an item in set
		new_state = context_free_grammar_create(grammar->desc_table);
		linked_list_node_type *prod_node = state->productions->head;
	
		while (prod_node != NULL)
		{
			// search "DOT" in the production's body
			production_type *prod = prod_node->data;
			production_token_type dot = DOT;
			linked_list_node_type *dot_node = linked_list_search(prod->body, &dot, int_equal, NULL);
				
			if (dot_node != NULL && dot_node->next != NULL)
			{
				production_type *prod_copy = production_copy(prod, NULL);
				dot_node = linked_list_search(prod_copy->body, &dot, int_equal, NULL);
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
				else
				{
					production_deconstructor(prod_copy, NULL);
				}
			}

			prod_node = prod_node->next;
		}

		if (new_state->productions->head == NULL)
		{
			context_free_grammar_destroy(new_state, NULL);
			new_state = NULL;

			free(key_pair);
		}
		else
		{
			new_state = LR_automata_closure(new_state, grammar);

			// update goto table
			// LR_table_key_pair_type *key_pair = (LR_table_key_pair_type *)malloc(sizeof(LR_table_key_pair_type));
			// key_pair->state = state;
			// key_pair->symbol = symbol;

			array_list_node_type *state_in_cc = array_list_search(lr_automata->items, new_state, context_free_grammar_comparator, NULL);
			if (state_in_cc == NULL)
			{
				hash_table_insert(lr_automata->goto_table, key_pair, new_state);
			}
			else
			{
				hash_table_insert(lr_automata->goto_table, key_pair, state_in_cc->data);
				
				context_free_grammar_deconstructor(new_state, NULL);

				return state_in_cc->data;
			}
			
		}
	}
	else
	{
		free(key_pair);
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

	char *prod_debug_str = production_debug_str(initial_production, grammar->desc_table);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "initial production: %s", prod_debug_str);
	free(prod_debug_str);

	// add the generated initial production to the given context-free grammar, as the new "first productoin".
	// in the regexp case, it is adding the production "Goal -> Regexp" to the first place of grammar
	linked_list_node_type *intial_production_node = linked_list_node_create();
	intial_production_node->data = initial_production;
	linked_list_insert_before(grammar->productions, grammar->productions->head, intial_production_node);
	
	char *cfg_debug_str = get_context_free_grammar_debug_str(grammar);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "the context-free grammar pass to LR automata: \n%s", cfg_debug_str);
	free(cfg_debug_str);

	lr_automata->first_set = LR_automata_construct_first_set(grammar);
	char *first_set_debug_str = get_set_debug_str(lr_automata->first_set, grammar->desc_table);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "FIRST(): %s", first_set_debug_str);
	free(first_set_debug_str);

	lr_automata->follow_set = LR_automata_construct_follow_set(lr_automata->first_set, grammar);
	char *follow_set_debug_str = get_set_debug_str(lr_automata->follow_set, grammar->desc_table);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "FOLLOW(): %s", follow_set_debug_str);
	free(follow_set_debug_str);
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

	char *cc0_debug_str = get_context_free_grammar_debug_str(cc0);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "cc0: %s", cc0_debug_str);
	free(cc0_debug_str);

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
	string_buffer_destroy(symbol_str);

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
	string_buffer_destroy(non_terminal_symbols_str);


	int last_iter_cc_size = 0;

	lr_automata->items = cc;

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
				// context_free_grammar_type *next_state = NULL;

				char *state_debug_str = get_context_free_grammar_debug_str(set);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE && LR_AUTOMATA_GOTO_TABLE_LOG_ENABLE, "state:\n%s", state_debug_str);
				free(state_debug_str);

				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE && LR_AUTOMATA_GOTO_TABLE_LOG_ENABLE, "symbol: %s", grammar->desc_table[*TYPE_CAST(grammar_symbol, int *)]);
				
				char *next_state_debug_str = get_context_free_grammar_debug_str(next_state);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE && LR_AUTOMATA_GOTO_TABLE_LOG_ENABLE, "next_state:\n%s", next_state_debug_str);
				free(next_state_debug_str);
				
				if (next_state != NULL && array_list_search(cc, next_state, context_free_grammar_comparator, NULL) == NULL)
				{
					array_list_append(cc, next_state);
				}
			}
		}

		last_iter_cc_size = cc->length;
	}
	

	char *cc_debug_str = get_array_list_debug_str(cc, context_free_grammar_debug_str, NULL);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "state count: %d, cc: \n%s\n", cc->length, cc_debug_str);
	free(cc_debug_str);

	char *goto_table_debug_str = get_hash_table_debug_str(lr_automata->goto_table, lr_table_key_pair_debug_str, context_free_grammar_debug_str, NULL);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_CONSTRUCT_SET_LOG_ENABLE, "GOTO TABLE: \n%s\n", goto_table_debug_str);
	free(goto_table_debug_str);

	linked_list_destroy(grammar_symbols, NULL);
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
						LR_table_key_pair_type *key = (LR_table_key_pair_type *)malloc(sizeof(LR_table_key_pair_type));
						key->state = state;
						key->symbol = symbol_next_to_dot;

						char *state_debug_str = get_context_free_grammar_debug_str(key->state);
						LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "SHIFT, state: %s, symbol: %s", state_debug_str, state->desc_table[*TYPE_CAST(key->symbol, int *)]);
						free(state_debug_str);

						action_table_value_type *value = (action_table_value_type *)malloc(sizeof(action_table_value_type));
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
						LR_table_key_pair_type *key = (LR_table_key_pair_type *)malloc(sizeof(LR_table_key_pair_type));
						key->state = state;
						production_token_type dollar_symbol = LR_automata_dollar_symbol;
						key->symbol = &LR_automata_dollar_symbol;

						action_table_value_type *value = (action_table_value_type *)malloc(sizeof(action_table_value_type));
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
							LR_table_key_pair_type *key = (LR_table_key_pair_type *)malloc(sizeof(LR_table_key_pair_type));
							key->state = state;
							key->symbol = symbol_node->data;

							action_table_value_type *value = (action_table_value_type *)malloc(sizeof(action_table_value_type));
							value->action = REDUCE;
							value->next_state = NULL;
							value->prod_to_reduce = prod;
							// value->prod_to_reduce = production_copy(prod, NULL);

							hash_table_insert(lr_automata->action_table, key, value);
						}
						

					}
				}
			} 
		}
	}

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "============================================ ACTION table constructed ===========================================");
	char *action_table_debug_str = get_hash_table_debug_str(lr_automata->action_table, lr_table_key_pair_debug_str, action_table_value_debug_str, NULL);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_ACTION_TABLE_LOG_ENABLE, "ACTION TABLE:\n%s", action_table_debug_str);
	free(action_table_debug_str);
}

LR_automata_type *
LR_automata_create(context_free_grammar_type *grammar)
{
	LR_automata_type *lr_automata = (LR_automata_type *)malloc(sizeof(LR_automata_type));
	lr_automata->items = NULL;
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
LR_automata_destroy(LR_automata_type *lr_automata, ...)
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

Ast_type *
LR_automata_parse(LR_automata_type *lr_automata, LR_automata_input_buffer_type *buffer, void (*callback)(stack_type *ast_node_stack, production_type *prod_to_reduce))
{
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_PARSE_LOG_ENABLE, "=============================== Start LR_automata_parse() ===============================");
	
	assert(buffer);

	// Create a empty AST:
	Ast_type *ast = Ast_create();

	// Initially, parser has s0 on its state stack
	stack_type *stack = stack_create();
	stack_push(stack, array_list_get(lr_automata->items, 0));

	// stack to hold AST nodes. This stack has exactly the same elements as state stack.
	stack_type *ast_node_stack = stack_create();

	LR_automata_input_type *lookup_symbol = LR_automata_input_buffer_read(buffer);
	production_token_type *lookup_symbol_type = &lookup_symbol->type;
	assert(lookup_symbol);

	while (TRUE && lookup_symbol != NULL)
	{	

		context_free_grammar_type *state = stack_peek(stack);

		char *state_debug_str = get_context_free_grammar_debug_str(state);
		LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_PARSE_LOG_ENABLE, "state: %s, symbol: %s \"%c\"", state_debug_str, lr_automata->grammar->desc_table[*TYPE_CAST(lookup_symbol_type ,int *)], lookup_symbol->c);
		free(state_debug_str);

		action_table_value_type *action_value = LR_automata_action(lr_automata, state, lookup_symbol_type);
		

		assert(action_value != NULL);
		
		if (action_value->action == SHIFT)
		{
			context_free_grammar_type *next_state = action_value->next_state;
			stack_push(stack, next_state);
			stack_push(ast_node_stack, lookup_symbol);

			lookup_symbol = LR_automata_input_buffer_read(buffer);
			lookup_symbol_type = lookup_symbol ? &(lookup_symbol->type) : NULL;

			char *next_state_debug_str = get_context_free_grammar_debug_str(next_state);
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_PARSE_LOG_ENABLE, "SHIFT to: %s\n", next_state_debug_str);
			free(next_state_debug_str);
		}
		else if (action_value->action == REDUCE)
		{
			char *reduce_debug_str = production_debug_str(action_value->prod_to_reduce, lr_automata->grammar->desc_table);
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_PARSE_LOG_ENABLE, "REDUCE %s\n", reduce_debug_str);
			free(reduce_debug_str);
			
			int reduced_production_size = 0;

			/*
			 * Calculate the reduced production size, which could decide our stack pop number.
			 */
			linked_list_node_type *node = NULL;
			for (node = action_value->prod_to_reduce->body->head; node != NULL; node = node->next)
			{
				production_token_type *body_token = node->data;
				int dot_symbol = DOT;
				if (!int_comparator(body_token, &dot_symbol, NULL))
				{
					reduced_production_size ++;
				}
			}

			/*
			 * Pop "reduced_production_size" elements.
			 */
			int i;
			for (i = 0; i < reduced_production_size; i ++)
			{
				stack_pop(stack);
			}

			// Call "callback" to construct the AST.
			if (callback != NULL)
			{
				callback(ast_node_stack, action_value->prod_to_reduce);
			}

			stack_push(stack, LR_automata_goto(lr_automata, stack_peek(stack), action_value->prod_to_reduce->head));
		}
		else if (action_value->action == ACCEPT)
		{
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_PARSE_LOG_ENABLE, "ACCEPT");
			break;
		}
		else
		{
			// Oops, error here!
		}
	}

	assert(ast_node_stack->length == 1);
	ast->root = stack_pop(ast_node_stack);

	stack_destroy(stack, NULL);
	stack_destroy(ast_node_stack, NULL);

	char *ast_debug_str = get_Ast_debug_str(ast, NULL);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_PARSE_LOG_ENABLE, "AST: %s", ast_debug_str);
	free(ast_debug_str);

	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_PARSE_LOG_ENABLE, "=============================== End LR_automata_parse() ===============================");

	return ast;
}

action_table_value_type *
LR_automata_action(LR_automata_type *lr_automata, context_free_grammar_type *state, production_token_type *symbol)
{
	hash_table_type *action_table = lr_automata->action_table;

	LR_table_key_pair_type *key = (LR_table_key_pair_type *)malloc(sizeof(LR_table_key_pair_type));
	key->state = state;
	key->symbol = symbol;

	action_table_value_type *ret = hash_table_search(action_table, key, lr_table_key_pair_comparator, NULL);
	free(key);
	return ret;
}

context_free_grammar_type *
LR_automata_goto(LR_automata_type *lr_automata, context_free_grammar_type *state, production_token_type *symbol)
{
	hash_table_type *goto_table = lr_automata->goto_table;

	LR_table_key_pair_type *key = (LR_table_key_pair_type *)malloc(sizeof(LR_table_key_pair_type));
	key->state = state;
	key->symbol = symbol;

	context_free_grammar_type *ret = hash_table_search(goto_table, key, lr_table_key_pair_comparator, NULL);
	free(key);
	return ret;
}

LR_automata_input_buffer_type *
LR_automata_input_buffer_create()
{
	LR_automata_input_buffer_type *buffer = (LR_automata_input_buffer_type *)malloc(sizeof(LR_automata_input_buffer_type));

	buffer->list = array_list_create();
	buffer->cursor = -1;
	buffer->get_token_type = NULL;

	return buffer;
}

void
LR_automata_input_buffer_init(LR_automata_input_buffer_type *buffer, char *input, production_token_type (*get_token_type)(char))
{
	if (input == NULL) return;

	buffer->get_token_type = get_token_type;

	int input_size = strlen(input);
	
	int i;
	for (i = 0; i < input_size; i ++)
	{
		LR_automata_input_type *element = (LR_automata_input_type *)malloc(sizeof(LR_automata_input_type));
		element->c = input[i];
		element->type = buffer->get_token_type(input[i]);

		array_list_append(buffer->list, element);
	}

	LR_automata_input_type *element = (LR_automata_input_type *)malloc(sizeof(LR_automata_input_type));
	element->c = '$';
	element->type = DOLLAR;

	array_list_append(buffer->list, element);
}

LR_automata_input_type *
LR_automata_input_buffer_read(LR_automata_input_buffer_type *buffer)
{
	if (buffer->cursor < 0 || buffer->cursor < buffer->list->length - 1)
	{
		++ (buffer->cursor);
		return array_list_get(buffer->list, buffer->cursor);
	}
	else
	{
		return NULL;
	}
}

bool
LR_automata_input_deconstructor(LR_automata_input_type *input, va_list arg_list)
{
	free(input);
	return TRUE;
}

bool
LR_automata_input_buffer_deconstructor(LR_automata_input_buffer_type *buffer, va_list arg_list)
{
	array_list_destroy(buffer->list, LR_automata_input_deconstructor, NULL);
	free(buffer);
	return TRUE;
}

bool
LR_automata_deconstructor(LR_automata_type *lr_automata, va_list arg_list)
{
	if (lr_automata == NULL)
	{
		return FALSE;
	}

	array_list_destroy(lr_automata->items, context_free_grammar_deconstructor, NULL);

	hash_table_destroy(lr_automata->goto_table, table_key_deconstructor, NULL);
	hash_table_destroy(lr_automata->action_table, table_key_deconstructor, action_table_value_deconstructor, NULL);

	array_list_destroy(lr_automata->follow_set, linked_list_deconstructor, NULL);
	array_list_destroy(lr_automata->first_set, linked_list_deconstructor, NULL);

	context_free_grammar_deconstructor(lr_automata->grammar, NULL);

	linked_list_deconstructor(lr_automata->non_terminal_symbols, NULL);

	free(lr_automata);

	return TRUE;
}

char *lr_table_key_pair_debug_str(LR_table_key_pair_type *key_pair, va_list arg_list)
{
	assert(key_pair != NULL);

	string_buffer debug_str = string_buffer_create();

	char *tmp = context_free_grammar_debug_str(key_pair->state, NULL);

	string_buffer_append(&debug_str, tmp);
	string_buffer_append(&debug_str, " , ");
	string_buffer_append(&debug_str, desc_table[*TYPE_CAST(key_pair->symbol ,int *)]);

	free(tmp);

	return debug_str;
}

bool
lr_table_key_pair_comparator(void *k1, void *k2, va_list arg_list)
{
	LR_table_key_pair_type *key1 = TYPE_CAST(k1, LR_table_key_pair_type *);
	LR_table_key_pair_type *key2 = TYPE_CAST(k2, LR_table_key_pair_type *);

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
action_table_value_debug_str(action_table_value_type *value, va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	if (value->action == ACCEPT)
	{
		string_buffer_append(&debug_str, "Accept ");
		string_buffer_append(&debug_str, "\n\n");
	}
	else if (value->action == SHIFT)
	{
		char *tmp = get_context_free_grammar_debug_str(value->next_state);

		string_buffer_append(&debug_str, "Shift to ");
		string_buffer_append(&debug_str, tmp);
		string_buffer_append(&debug_str, "\n\n");

		free(tmp);
	}
	else if (value->action == REDUCE)
	{
		char *tmp = production_debug_str(value->prod_to_reduce, desc_table);

		string_buffer_append(&debug_str, "Reduce to ");
		string_buffer_append(&debug_str, tmp);
		string_buffer_append(&debug_str, "\n\n");

		free(tmp);
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
	int *goal_symbol = &LR_automata_goal_symbol;
	int *dollar_symbol = &LR_automata_dollar_symbol;

	linked_list_type *update_set = linked_list_create();
	linked_list_insert_back(update_set, dollar_symbol);

	LR_automata_set_update(follow_set, goal_symbol, update_set);
	linked_list_destroy(update_set, NULL);

	char *initial_follow_set_debug_str = get_set_debug_str(follow_set, grammar->desc_table);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "initial FOLLOW set: %s", initial_follow_set_debug_str);
	free(initial_follow_set_debug_str);

	do {
		
		linked_list_node_type *prod_node = NULL;
		for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
		{
			production_type *prod = prod_node->data;

			char *process_debug_str = production_debug_str(prod, grammar->desc_table);
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "process %s", process_debug_str);
			free(process_debug_str);

			linked_list_type *trailer = linked_list_create();

			char *sub_follow_set_debug_str = get_sub_set_debug_str(LR_automata_follow(follow_set, prod->head), grammar->desc_table);
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "FOLLOW(%s): %s", grammar->desc_table[*TYPE_CAST(prod->head, int *)], sub_follow_set_debug_str);
			free(sub_follow_set_debug_str);

			linked_list_merge(trailer, LR_automata_follow(follow_set, prod->head), int_comparator, address_assign, NULL);

			linked_list_node_type *prod_token_node;
			for (prod_token_node = prod->body->tail; prod_token_node != NULL; prod_token_node = prod_token_node->prev)
			{
				production_token_type *prod_token = prod_token_node->data;

				if (linked_list_search(non_terminal_symbols, prod_token, int_comparator, NULL) != NULL)
				{
					bool has_changed = LR_automata_set_update(follow_set, prod_token, trailer);

					char *update_sub_follow_set_str = get_set_debug_str(follow_set, grammar->desc_table);
					LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "update FOLLOW(%s): %s", grammar->desc_table[*TYPE_CAST(prod_token, int *)], update_sub_follow_set_str);
					free(update_sub_follow_set_str);

					follow_set_has_changed = has_changed ? TRUE : has_changed;

					linked_list_type *first_set_of_prod_token = LR_automata_first(first_set, prod_token);
					production_token_type epsilon_symbol = EPSILON;

					char *first_set_debug_str = get_sub_set_debug_str(first_set_of_prod_token, grammar->desc_table);
					LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "FIRST(%s): %s", grammar->desc_table[*TYPE_CAST(prod_token, int *)], first_set_debug_str);
					free(first_set_debug_str);

					if (linked_list_search(first_set_of_prod_token, &epsilon_symbol, int_comparator, NULL))
					{
						// add everthing but epsilon to trailer
						linked_list_type *first_set_but_epsilon = linked_list_create();
						linked_list_node_type *n;
						for (n = first_set_of_prod_token->head; n != NULL; n = n->next)
						{
							if (!int_comparator(n->data, &epsilon_symbol, NULL))
							{
								linked_list_insert_back(first_set_but_epsilon, n->data);
							}
						}

						linked_list_merge(trailer, first_set_but_epsilon, int_comparator, address_assign, NULL);
						linked_list_destroy(first_set_but_epsilon, NULL);

						char *trailer_after_merge_debug_str = get_sub_set_debug_str(trailer, grammar->desc_table);
						LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "merge, trailer: %s", trailer_after_merge_debug_str);
						free(trailer_after_merge_debug_str);
					}
					else
					{
						linked_list_destroy(trailer, NULL);
						trailer = linked_list_copy(first_set_of_prod_token, address_assign, NULL);

						char *trailer_after_truncate_debug_str = get_sub_set_debug_str(trailer, grammar->desc_table);
						LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "truncate, trailer: %s", trailer_after_truncate_debug_str);
						free(trailer_after_truncate_debug_str);
					}

				}
				else
				{
					linked_list_destroy(trailer, NULL);
					trailer = linked_list_create();
					linked_list_merge(trailer, LR_automata_first(first_set, prod_token), int_comparator, address_assign, NULL);

					char *non_term_debug_str = get_sub_set_debug_str(trailer, grammar->desc_table);
					LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "non-terminal, trailer: %s", non_term_debug_str);
					free(non_term_debug_str);
				}
			}

			linked_list_destroy(trailer, NULL);
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "process done ==============================================================================");

		}
	} while (follow_set_has_changed == TRUE);
	
	linked_list_destroy(non_terminal_symbols, NULL);

	char *follow_set_debug_str = get_set_debug_str(follow_set, grammar->desc_table);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FOLLOW_SET_LOG_ENABLE, "follow set: \n  %s", follow_set_debug_str);
	free(follow_set_debug_str);

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

	char *init_first_set_debug_str = get_set_debug_str(first_set, grammar->desc_table);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "initialized first set:\n%s\n", init_first_set_debug_str);
	free(init_first_set_debug_str);

	linked_list_destroy(symbol_list, NULL);

	do {
		first_set_has_changed = FALSE;

		linked_list_node_type *prod_node;
		for (prod_node = grammar->productions->head; prod_node != NULL; prod_node = prod_node->next)
		{
			production_type *prod = prod_node->data;
			assert(prod);

			linked_list_type *first_set_of_current_symbol = linked_list_create();

			char *prod_debug_str = production_debug_str(prod, grammar->desc_table);
			LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "from the production %s", prod_debug_str);
			free(prod_debug_str);

			linked_list_node_type *body_node;
			for (body_node = prod->body->head; body_node != NULL; body_node = body_node->next)
			{

				linked_list_type *first_set_of_b1 = array_list_get(first_set, *TYPE_CAST(body_node->data, int *));
				char *sub_set_debug_str = get_sub_set_debug_str(first_set_of_b1, grammar->desc_table);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "first set of %s is: %s", grammar->desc_table[*TYPE_CAST(body_node->data, int *)], sub_set_debug_str);
				free(sub_set_debug_str);

				//
				// every item in FIRST(Y[i]), except for "epsilon":
				//

				linked_list_type *first_set_of_b1_without_epsilon = linked_list_create();

				linked_list_node_type *node;
				for (node = first_set_of_b1->head; node != NULL; node = node->next)
				{
					if (int_comparator(node->data, &epsilon, NULL) == FALSE) 
					{
						linked_list_insert_back(first_set_of_b1_without_epsilon, node->data);
					}
				}

				sub_set_debug_str = get_sub_set_debug_str(first_set_of_b1_without_epsilon, grammar->desc_table);
				LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "exclude epsilon: %s", sub_set_debug_str);
				free(sub_set_debug_str);

				//
				// put every item in FIRST(Y[i]), except for "epsilon", into FIRST(A):
				//

				linked_list_merge(first_set_of_current_symbol, first_set_of_b1_without_epsilon, int_comparator, address_assign, NULL);
				linked_list_destroy(first_set_of_b1_without_epsilon, NULL);

				if (linked_list_search(first_set_of_b1, &epsilon, int_comparator, NULL) == NULL) break;

				if (body_node == prod->body->tail && linked_list_search(first_set_of_b1, &epsilon, int_comparator, NULL))
				{	
					// if "epsilon" in all Y[0] ~ Y[k], place "epsilon" in FIRST(A)
					linked_list_insert_back(first_set_of_current_symbol, &LR_automata_epsilon_symbol);
				}
			}

			bool has_change = LR_automata_set_update(first_set, prod->head, first_set_of_current_symbol);
			if (has_change)
			{
				first_set_has_changed = TRUE;
			}

			linked_list_destroy(first_set_of_current_symbol, NULL);
			
		}

		char *first_set_this_round_debug_str = get_set_debug_str(first_set, grammar->desc_table);
		LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "first set, this round:\n%s\n", first_set_this_round_debug_str);
		free(first_set_this_round_debug_str);

	} while (first_set_has_changed);

	linked_list_destroy(non_terminal_symbols, NULL);

	char *first_set_debug_str = get_set_debug_str(first_set, grammar->desc_table);
	LOG(LR_AUTOMATA_LOG_ENABLE && LR_AUTOMATA_FIRST_SET_LOG_ENABLE, "first set:\n%s\n", first_set_debug_str);
	free(first_set_debug_str);

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

	return linked_list_merge(follow_set_of_symbol, update_set, int_comparator, address_assign, NULL);
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
