#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "util/linked_list.h"
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
bool
linked_list_destroy(linked_list_type *list, void (*data_deconstructor)(void *))
{
	assert(list);
	
	linked_list_node_type *node = list->head;

	// destroy all nodes
	while (node != NULL)
	{
		linked_list_node_type *next = node->next;

		if (data_deconstructor != NULL)
		{
			data_deconstructor(node->data);
		}
		
		free(node);

		node = next;
	}

	// destroy list
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
linked_list_node_destroy(linked_list_node_type *node, void (*data_deconstructor)(void *))
{
	assert(node);
	
	if (data_deconstructor)	
	{
		data_deconstructor(node->data);	
	}
	
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
linked_list_delete(linked_list_type *list, linked_list_node_type *node, void (*data_deconstructor)(void *))
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

	linked_list_node_destroy(node, data_deconstructor);	

	return TRUE;
}


char *
linked_list_debug_str(linked_list_type *list, char * (*data_to_str)(void *data))
{
	// Initialize "debug_str", 5 for "[  ]\0"
	int size_of_debug_str = 5;
	char *debug_str = (char *)malloc(sizeof(char) * size_of_debug_str);
	debug_str[0] = '[';
	debug_str[1] = ' ';
	
	linked_list_node_type *node = list->head;
	
	while (node != NULL)
	{
		char *node_text = data_to_str(node->data);		
		
		if (node_text == NULL)
		{
			node_text = (char *)malloc(sizeof(char) * (strlen("NULL") + 1));
			strcpy(node_text, "NULL");
		}
		
		// extra 2 size is for ", "
		size_of_debug_str += (strlen(node_text) + 2);
		if (node->next == NULL) size_of_debug_str -= 2;

		// reallocate memory
		debug_str = (char *)realloc(debug_str, size_of_debug_str);
		 
		// copy "node_text" to "debu_str"
		strcat(debug_str, node_text);
		
		if (node->next != NULL)
			strcat(debug_str, ", ");
		
		free(node_text);

		node = node->next;
	}
	
	DB_LOG(TRUE, "%d", size_of_debug_str);
	debug_str[size_of_debug_str - 3] = ' ';
	debug_str[size_of_debug_str - 2] = ']';
	debug_str[size_of_debug_str - 1] = '\0';

	return debug_str;
		
}










