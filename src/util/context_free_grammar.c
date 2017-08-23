#include "context_free_grammar.h"
#include "util.h"
#include "logger.h"
#include "string_buffer.h"
#include "linked_list.h"

#include <stdarg.h>
#include <assert.h>

context_free_grammar_type *
context_free_grammar_create(char **desc_table)
{
	context_free_grammar_type *grammar = (context_free_grammar_type *)malloc(sizeof(context_free_grammar_type));
	grammar->productions = linked_list_create();
	grammar->desc_table = desc_table;

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

	linked_list_destroy(grammar->productions, production_deconstructor, NULL);

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

	if (prod->head != NULL)
		free(prod->head);
	
	linked_list_destroy(prod->body, int_deconstructor, NULL);

	free(prod);

	return FALSE;
}


void
context_free_grammar_add(context_free_grammar_type *grammar, int head_value, ...)
{
	assert(grammar != NULL && head_value != 0);

	production_type *production = production_create();
	production->head = create_int(0);
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

	linked_list_insert_back(grammar->productions, production);
}

void
context_free_grammar_add_production(context_free_grammar_type *grammar, production_type *prod)
{	
	assert(grammar != NULL && prod != NULL);

	linked_list_insert_back(grammar->productions, prod);
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
get_context_free_grammar_debug_str(context_free_grammar_type *grammar)
{
	if (grammar == NULL || grammar->productions == NULL)
	{
		return NULL;
	}
	
	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, "[");

	linked_list_type *list = grammar->productions;

	linked_list_node_type *node = list->head;

	while (node != NULL)
	{
		char *item_str = production_debug_str(node->data, grammar->desc_table);

		string_buffer_append(&debug_str, item_str);
		string_buffer_destroy(item_str);

		string_buffer_append(&debug_str, ", ");
		
		node = node->next;
	}

	string_buffer_append(&debug_str, "]");

	return debug_str;
}

production_type *
production_copier(production_type *prod, va_list arg_list)
{
	production_type *prod_copy = production_create();
	prod_copy->head = create_int(*TYPE_CAST(prod->head, int *));
	prod_copy->body = linked_list_copy(prod->body, int_copier, NULL);

	return prod_copy;
}

context_free_grammar_type *
context_free_grammar_copier(context_free_grammar_type *grammar, va_list arg_list)
{
	if (grammar == NULL)
	{
		return NULL;
	}

	context_free_grammar_type *grammar_copy = context_free_grammar_create(grammar->desc_table);
	grammar_copy->productions = linked_list_copy(grammar->productions, production_copier, NULL);

	return grammar_copy;
}

bool
context_free_grammar_comparator(void *raw_p1, void *raw_p2, va_list arg_list)
{
	context_free_grammar_type *p1 = TYPE_CAST(raw_p1, context_free_grammar_type *);
	context_free_grammar_type *p2 = TYPE_CAST(raw_p2, context_free_grammar_type *);

	if (p1 == p2) return TRUE;
	if (p1 == NULL || p2 == NULL)
	{
		if (p1 == NULL && p2 == NULL)
			return TRUE;
		else
			return FALSE;
	}

	return linked_list_compare(p1->productions, p2->productions, production_comparator, NULL);
}

linked_list_node_type *
production_search_symbol(production_type *prod, production_token_type *symbol)
{
	assert(prod && symbol);

	return linked_list_search(prod->body, symbol, int_comparator, NULL);
}

production_type *
context_free_grammar_search_production(context_free_grammar_type *grammar, production_type *production)
{
	assert(grammar && production);

	if (grammar->productions == NULL) return NULL;

	linked_list_node_type *prod_node = grammar->productions->head;

	while (prod_node != NULL)
	{
		production_type *prod = (production_type *)prod_node->data;


		prod_node = prod_node->next;
	}

	return NULL;
}

bool
production_comparator(void *raw_p1, void *raw_p2, va_list arg_list)
{
	production_type *p1 = TYPE_CAST(raw_p1, production_type *);
	production_type *p2 = TYPE_CAST(raw_p2, production_type *);

	if (p1 == p2) return TRUE;
	if (p1 == NULL || p2 == NULL)
	{
		if (p1 == NULL && p2 == NULL)
			return TRUE;
		else
			return FALSE;
	}

	return int_comparator(p1->head, p2->head, NULL) && linked_list_compare(p1->body, p2->body, int_comparator, NULL);
}