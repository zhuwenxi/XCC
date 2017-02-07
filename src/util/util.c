#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

linked_list_t *
linked_list_create()
{
	linked_list_t *self = malloc(sizeof (linked_list_t));
	self->head = NULL;
	self->tail= NULL;
	return self; 
}

void
linked_list_destroy(linked_list_t *list, void (*node_destructor)())
{
	linked_list_node_t *current_node;
	linked_list_node_t *next_node = list->head;
	
	// De-construct every node in this list.
	while (next_node != NULL) 
	{	
		current_node = next_node;

		// De-construct "data" field, with "node_destructor()".
		if (node_destructor != NULL) {
			void *data = current_node->data;
			node_destructor(data);
		}	
		
		next_node = current_node->next; 
		
		// De-construct current node.
		free(current_node);
	}	

	// De-consturct the list structure.
	free(list);

}

linked_list_node_t *
linked_list_first_item(linked_list_t *list)
{
	return list->head;
}

linked_list_node_t *
linked_list_last_item(linked_list_t *list)
{
	return list->tail;
}

linked_list_node_t *
linked_list_limit(linked_list_t *list)
{
	return (linked_list_node_t *)list;
}

linked_list_node_t *
linked_list_next(linked_list_node_t *node)
{
	return node->next;
}

linked_list_node_t *
linked_list_prev(linked_list_node_t *node)
{
	return node->prev;
}

bool linked_list_append(linked_list_t *list, void *data)
{
	if (list->head == NULL) {
		// Case 1: append element to an empty list.

		assert(list->tail == NULL);
		
		// Create a new node to hold the data.
		linked_list_node_t *new_node = (linked_list_node_t *)malloc(sizeof(linked_list_node_t));	
		new_node->data = data;
		new_node->next = NULL;
		new_node->prev = NULL;
		
		// Append this node to the list.
		list->head = new_node;
		list->tail = new_node;
	} else {
		// Case 2: append element to a nonempty	list.

		assert(list->tail != NULL);

		// Create a new node to hold the data.
		linked_list_node_t *new_node = (linked_list_node_t *)malloc(sizeof(linked_list_node_t));	
		new_node->data = data;
		new_node->next = NULL;
		new_node->prev = list->tail;

		// Let the the new_node be the last node.
		list->tail->next = new_node;
		list->tail = new_node;
	}
}
