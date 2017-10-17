#ifndef __LR_AUTOMATA_H__
#define __LR_AUTOMATA_H__

#include "array_list.h"
#include "hash_table.h"
#include "context_free_grammar.h"
#include "stddefs.h"
#include "Ast.h"

#include <stdarg.h>

typedef struct
{
	// states
	array_list_type *items;

	// goto & action table
	hash_table_type *goto_table;
	hash_table_type *action_table;

	// follow & first set
	array_list_type *follow_set;
	array_list_type *first_set;

	context_free_grammar_type *grammar;
	// record all non-terminal symbols
	linked_list_type *non_terminal_symbols;
} LR_automata_type;

typedef struct
{
	context_free_grammar_type *state;
	production_token_type *symbol;
} lr_table_key_pair_type;

typedef enum
{
	SHIFT,
	REDUCE,
	ACCEPT,
} action_type;

typedef struct
{
	action_type action;
	context_free_grammar_type *next_state;
	production_type *prod_to_reduce;
} action_table_value;

LR_automata_type *LR_automata_create(context_free_grammar_type *grammar);

bool LR_automata_destory(LR_automata_type *lr_automata, ...);

bool LR_automata_deconstructor(LR_automata_type *lr_automata, va_list arg_list);

array_list_type *LR_automata_construct_follow_set(array_list_type *first_set, context_free_grammar_type *grammar);
array_list_type *LR_automata_construct_first_set(context_free_grammar_type *grammar);
bool LR_automata_set_update(array_list_type *follow_set, production_token_type *symbol, linked_list_type *update_set);

linked_list_type *LR_automata_follow(array_list_type *follow_set, production_token_type *symbol);
linked_list_type *LR_automata_first(array_list_type *first_set, production_token_type *symbol);

Ast_type *LR_automata_parse(LR_automata_type *lr_automata, char *text, production_token_type (*get_token_type)(char *));

int key_pair_hash(void *key_pair);

char *lr_table_key_pair_debug_str(lr_table_key_pair_type *key_pair, va_list arg_list);

bool lr_table_key_pair_comparator(void *key1, void *key2, va_list arg_list);

char *action_table_value_debug_str(action_table_value *value, va_list arg_list);

static char *get_set_debug_str(array_list_type *set, char *desc_table[]);
static char *get_sub_set_debug_str(linked_list_type *set, char *desc_table[]);
#endif