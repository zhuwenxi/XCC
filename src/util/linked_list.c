#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "util/linked_list.h"
#include "util/string_buffer.h"
#include "stddefs.h"
#include "opts.h"
#include "util/logger.h"





// create list
linked_list_type *
linked_list_create()
{
	linked_list_type *list = (linked_list_type *)malloc(sizeof(linked_list_type));
	list->head = NULL;
	list->tail = NULL;
	return list;
}

// destroy list
// bool
// linked_list_destroy(linked_list_type *list, void (*data_deconstructor)(void *))
// {
// 	assert(list);
	
// 	linked_list_node_type *node = list->head;

// 	// destroy all nodes
// 	while (node != NULL)
// 	{
// 		linked_list_node_type *next = node->next;

// 		linked_list_node_destroy(node,data_deconstructor);
		
// 		free(node);

// 		node = next;
// 	}

// 	// destroy list
// 	free(list);

// 	return TRUE;
// }
bool
linked_list_destroy(linked_list_type *list, ...)
{
	if (list == NULL)
	{
		return FALSE;
	}

	va_list ap;
	va_start(ap, list);
	linked_list_deconstructor(list, ap);
	va_end(ap);

	return TRUE;
}

bool
linked_list_deconstructor(linked_list_type *list, va_list arg_list)
{
	if (list == NULL)
	{
		return FALSE;
	}

	linked_list_node_type *node = list->head;
	while (node != NULL)
	{
		linked_list_node_type *next_node = node->next;

		va_list tmp;
		va_copy(tmp, arg_list);
		linked_list_node_destroy(node, tmp);
		va_end(tmp);

		node = next_node;
	}

	free(list);

	return TRUE;
}

// create node
linked_list_node_type *
linked_list_node_create()
{
	linked_list_node_type *node = (linked_list_node_type *)malloc(sizeof(linked_list_node_type));
	node->data = NULL;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

// destroy node
bool 
linked_list_node_destroy(linked_list_node_type *node, va_list arg_list)
{
	bool (*sub_deconstructor)(void *, va_list);
	sub_deconstructor = va_arg(arg_list, bool (*)(void *, va_list));

	if (sub_deconstructor != NULL)
	{
		sub_deconstructor(node->data, arg_list);
	}

	free(node);

	return TRUE;
}

// insert at the tail of the list 
bool 
linked_list_insert_back(linked_list_type *list, void *data)
{
	assert(list != NULL);
	
	// create a new node, and set it's "data" field.
	linked_list_node_type *new_node = linked_list_node_create();
	new_node->data = data;
	list->tail = new_node;

	if (list->head == NULL)
	{
		list->head = new_node;
		new_node->prev = NULL;
		new_node->next = NULL;
	}
	else
	{
		linked_list_node_type *next_node = list->head;
		linked_list_node_type *last_node = NULL;

		while (next_node != NULL)
		{
			last_node = next_node;
			next_node = last_node ->next;
		}

		last_node->next = new_node;
		new_node->prev = last_node;
		new_node->next = NULL;
	}

	return TRUE;
}

bool
linked_list_insert(linked_list_type *list, linked_list_node_type *index_node, linked_list_node_type *node)
{
	assert(list && index_node && node);	

	// reset list->tail if necessary
	if(list->tail == index_node)
	{
		list->tail = node;
	}

	// cache the node next to "index_node"
	linked_list_node_type *next_to_index = index_node->next;

	// set node->prev and index_node->next
	node->prev = index_node;	
	index_node->next = node;

	// set node->next
	node->next = next_to_index;

	if (next_to_index != NULL)
	{
		next_to_index->prev = node;
	}
		
	return TRUE;
}

// search node in list
linked_list_node_type *
linked_list_search(linked_list_type *list, void *data, bool (*equal)(void *, void *))
{
	assert(list && data && equal);
	
	linked_list_node_type *node = list->head;
	while (node)
	{
		// find the element
		if (equal(node->data, data)) 	
		{
			return node;
		}

		node = node->next;
	}

	return NULL;
}

// delete node in list.
bool
linked_list_delete(linked_list_type *list, linked_list_node_type *node, ...)
{
	assert(list && node);

	linked_list_node_type *next_node = node->next;
	linked_list_node_type *prev_node = node->prev;
	

	/*
	 * prev_node->next = next_node;
	 */	
	if (prev_node == NULL)
	{
		// node == list->head	
		assert(list->head == node);
		
		list->head = next_node;
	}
	else
	{
		assert(list->head != node);

		prev_node->next = node->next;
	}

	/*
	 * next_node->prev = prev_node;
	 */
	if (next_node == NULL)
	{
		assert(list->tail == node);

		list->tail = prev_node;
	}
	else
	{
		assert(list->tail != node);

		next_node->prev = prev_node;
	}

	va_list arg_list;
	va_start(arg_list, node);
	linked_list_node_destroy(node, arg_list);
	va_end(arg_list);

	return TRUE;
}

char *
get_linked_list_debug_str(linked_list_type *list, ...)
{
	va_list arg_list;
	va_start(arg_list, list);

	char* debug_str = linked_list_debug_str(list, arg_list);

	va_end(arg_list);

	
	return debug_str;
}

char *
linked_list_debug_str(linked_list_type *list, va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, "[");

	va_list arg_list_copy;
	va_copy(arg_list_copy, arg_list);
	
	char *(*sub_debug_str)(void *, va_list);
	sub_debug_str = va_arg(arg_list_copy, char *(*)(void *, va_list));

	linked_list_node_type *node = list->head;
	while (node != NULL)
	{
		linked_list_node_type *next_node = node->next;

		char *item_str;
		if (sub_debug_str != NULL)
		{
			item_str = sub_debug_str(node->data, arg_list_copy);
			string_buffer_append(&debug_str, item_str);
			string_buffer_destroy(item_str);
		}
		else
		{
			item_str = (char *)node->data;
			string_buffer_append(&debug_str, item_str);
		}

		if (node != list->tail)
			string_buffer_append(&debug_str, " -> ");

		node = next_node;
	}

	va_end(arg_list_copy);

	string_buffer_append(&debug_str, "]");

	return debug_str;

}










