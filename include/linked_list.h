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
} linked_list_type;

linked_list_type *linked_list_create();
bool linked_list_destroy(linked_list_type *list);

#endif
