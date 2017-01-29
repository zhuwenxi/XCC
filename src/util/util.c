#include <stdlib.h>
#include "util.h"

linked_list_t *
linked_list_create()
{
	return malloc(sizeof (linked_list_t));
}

void
linked_list_destroy(linked_list_t *list, void (*node_destructor)())
{
	linked_list_node_t *node;
	
	if (node_destructor == NULL) {
		return;
	}

	for (node = linked_list_first_item(list); node != linked_list_limit(list); node = linked_list_next(node)){
		void *data = node->data; 
		free(data);
	}
	
	return;
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
