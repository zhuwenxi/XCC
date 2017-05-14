#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

// insert back
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














