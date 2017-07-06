#ifndef __CONTEXT_FREE_GRAMMAR_H__
#define __CONTEXT_FREE_GRAMMAR_H__

#include "array_list.h"
#include "linked_list.h"

#include <stdarg.h>

typedef struct
{
	char *head;
	linked_list_type *body;
} production_type;

typedef struct
{
	array_list_type *productions;
} context_free_grammar_type;

context_free_grammar_type *context_free_grammar_create();

bool context_free_grammar_destroy(context_free_grammar_type *grammar, ...);

bool context_free_grammar_deconstructor(context_free_grammar_type *grammar, va_list arg_list);

production_type *production_create();

bool production_deconstructor(production_type *prod, va_list arg_list);

void context_free_grammar_add(context_free_grammar_type *grammar, char *head, ...);

#endif