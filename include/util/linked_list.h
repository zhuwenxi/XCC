#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__




#include "stddefs.h"

/*
 * linked list node definition.
 */
typedef struct linked_list_node
{
	void *data;
	struct linked_list_node *prev;
	struct linked_list_node *next;
} linked_list_node_type;

/*
 * linked list definition
 */
typedef struct linked_list
{
	linked_list_node_type *head;
	linked_list_node_type *tail;
} linked_list_type;




/*
 * Create list.
 */
linked_list_type *linked_list_create();

/*
 * Destroy list.
 */
bool linked_list_destroy(linked_list_type *list);

/*
 * Create node.
 */
linked_list_node_type *linked_list_node_create();

/*
 * Destroy node.
 */
bool linked_list_node_destroy();

/*
 * Insert node to list.
 */
bool linked_list_insert(linked_list_type *list, int index, void *data);
bool linked_list_insert_back(linked_list_type *list, void *data);

/*
 * Search node in list.
 */
linked_list_node_type *linked_list_search(linked_list_type *list, void *data, bool (*equal)(void *, void *));

#endif
