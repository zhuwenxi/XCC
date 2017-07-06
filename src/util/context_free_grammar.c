#include "context_free_grammar.h"

context_free_grammar_type *
context_free_grammar_create()
{
	context_free_grammar_type *grammar = (context_free_grammar_type *)malloc(sizeof(context_free_grammar_type));
	grammar->productions = array_list_create();

	return grammar;
}

bool
context_free_grammar_destroy(context_free_grammar_type *grammar, ...)
{
	if (grammar == NULL)
	{
		return FALSE;
	}

	va_list arg_list;
	va_arg(arg_list, grammar);
	context_free_grammar_deconstructor(grammar, arg_list);
	va_end(arg_list);

	return TRUE;
}

bool
context_free_grammar_deconstructor(context_free_grammar_type *grammar, va_list arg_list)
{
	if (grammar == NULL)
	{
		return FALSE;
	}

	array_list_deconstructor(grammar->productions, arg_list);

	free(grammar);

	return FALSE;
}

production_type *
production_create()
{
	production_type *production = (production_type *)malloc(sizeof(production_type *));

	production->head = NULL;
	production->body = linked_list_create();

	return production;
}

bool
production_deconstructor(production_type *prod, va_list arg_list)
{
	if (prod == NULL)
	{
		return FALSE;
	}

	linked_list_destroy(prod->body, arg_list);

	free(prod);

	return FALSE;
}


void
context_free_grammar_add(context_free_grammar_type *grammar, char *head, ...)
{
	assert(grammar != NULL && head != NULL);

	production_type *production = production_create();
	production->head = head;

	

	array_list_append(array_list_grammar->productions, head);
}

