#ifndef __CONTEXT_FREE_GRAMMAR_H__
#define __CONTEXT_FREE_GRAMMAR_H__

#include "array_list.h"
#include "linked_list.h"

#include <stdarg.h>

typedef int production_token_type;

typedef struct
{
	production_token_type *head;
	linked_list_type *body;
} production_type;

typedef struct
{
	linked_list_type *productions;
	char **desc_table;
} context_free_grammar_type;

context_free_grammar_type *context_free_grammar_create(char **desc_table);

bool context_free_grammar_destroy(context_free_grammar_type *grammar, ...);

bool context_free_grammar_deconstructor(context_free_grammar_type *grammar, va_list arg_list);

production_type *production_create();

bool production_deconstructor(production_type *prod, va_list arg_list);

char *production_debug_str(production_type *prod, char **desc_table);

void context_free_grammar_add(context_free_grammar_type *grammar, int head_value, ...);

char *get_context_free_grammar_debug_str(context_free_grammar_type *grammar);

#endif