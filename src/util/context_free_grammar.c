#include "context_free_grammar.h"
#include "util.h"
#include "logger.h"
#include "string_buffer.h"
#include "linked_list.h"

#include <stdarg.h>
#include <assert.h>

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
	va_start(arg_list, grammar);
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

	array_list_destroy(grammar->productions, production_deconstructor);

	free(grammar);

	return FALSE;
}

production_type *
production_create()
{
	production_type *production = (production_type *)malloc(sizeof(production_type *));

	production->head = create_int(0);
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

	assert(prod->head != NULL && prod->body != NULL);

	free(prod->head);
	linked_list_destroy(prod->body, int_deconstructor);

	free(prod);

	return FALSE;
}


void
context_free_grammar_add(context_free_grammar_type *grammar, int head_value, ...)
{
	assert(grammar != NULL && head_value != 0);

	production_type *production = production_create();
	*(production->head) = head_value;

	va_list ap;
	va_start(ap, head_value);
	int body_token_value = va_arg(ap, int);
	while (body_token_value != 0)
	{	
		int *body_token = create_int(body_token_value);
		linked_list_insert_back(production->body, body_token);

		body_token_value = va_arg(ap, int);
	}
	va_end(ap);

	array_list_append(grammar->productions, production);
}

char *
production_debug_str(production_type *prod, char **desc_table)
{
	string_buffer debug_str = string_buffer_create();

	if (desc_table != NULL)
	{
		string_buffer_append(&debug_str, desc_table[*TYPE_CAST(prod->head, int *)]);
		string_buffer_append(&debug_str, " -> ");

		linked_list_node_type *node = prod->body->head;
		while (node != NULL)
		{
			int body_token = *TYPE_CAST(node->data, int *);
			string_buffer_append(&debug_str, desc_table[body_token]);
			string_buffer_append(&debug_str, " ");

			node = node->next;
		}

	}
	else
	{	
		char str[2] = {'\0', '\0'};

		str[0] = *TYPE_CAST(prod->head, int *) + '0';
		string_buffer_append(&debug_str, str);
		string_buffer_append(&debug_str, " -> ");

		linked_list_node_type *node = prod->body->head;
		while (node != NULL)
		{
			int body_token = *TYPE_CAST(node->data, int *);

			str[0] = body_token + '0';
			string_buffer_append(&debug_str, str);
			string_buffer_append(&debug_str, " ");

			node = node->next;
		}
	}


	return debug_str;
}

char *
get_context_free_grammar_debug_str(context_free_grammar_type *grammar, char **desc_table)
{
	if (grammar == NULL || grammar->productions == NULL)
	{
		return NULL;
	}
	
	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, "[");

	array_list_type *list = grammar->productions;

	int i;
	for (i = 0; i < list->length; i++)
	{
		char *item_str = production_debug_str(list->content[i]->data, desc_table); // sub_debug_str(list->content[i]->data, arg_list_copy);
		string_buffer_append(&debug_str, item_str);
		string_buffer_destroy(item_str);

		if (i != list->length - 1)
			string_buffer_append(&debug_str, ", ");
	}

	string_buffer_append(&debug_str, "]");

	return debug_str;
}
