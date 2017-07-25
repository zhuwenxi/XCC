#ifndef __LR_AUTOMATA_H__
#define __LR_AUTOMATA_H__

#include "array_list.h"
#include "hash_table.h"
#include "context_free_grammar.h"
#include "stddefs.h"

#include <stdarg.h>

// "Goal" indicates the desired non-terminial symbol.
#define Goal 0

typedef struct
{
	array_list_type *LR_items;
	hash_table_type *goto_table;
	hash_table_type *action_table;
} LR_automata_type;

typedef struct
{
	array_list_type *productions;
} LR_automata_item_type;

typedef struct
{
	LR_automata_item_type state;
	production_token_type symbol;
} key_pair_type;

LR_automata_type *LR_automata_create(context_free_grammar_type *grammar);

bool LR_automata_destory(LR_automata_type *lr_automata, ...);

bool LR_automata_deconstructor(LR_automata_type *lr_automata, va_list arg_list);

#endif