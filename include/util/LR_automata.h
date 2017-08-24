#ifndef __LR_AUTOMATA_H__
#define __LR_AUTOMATA_H__

#include "array_list.h"
#include "hash_table.h"
#include "context_free_grammar.h"
#include "stddefs.h"

#include <stdarg.h>

typedef struct
{
	array_list_type *items;
	hash_table_type *goto_table;
	hash_table_type *action_table;
} LR_automata_type;

typedef struct
{
	context_free_grammar_type *state;
	production_token_type *symbol;
} lr_table_key_pair_type;

LR_automata_type *LR_automata_create(context_free_grammar_type *grammar);

bool LR_automata_destory(LR_automata_type *lr_automata, ...);

bool LR_automata_deconstructor(LR_automata_type *lr_automata, va_list arg_list);

int key_pair_hash(void *key_pair);

char *lr_table_key_pair_debug_str(lr_table_key_pair_type *key_pair, va_list arg_list);
#endif