#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <stdarg.h>

#include "stddefs.h"
#include "util.h"

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
bool linked_list_destroy(linked_list_type *list, ...);

bool linked_list_deconstructor(linked_list_type *list, va_list arg_list); 

/*
 * Create node.
 */
linked_list_node_type *linked_list_node_create();

/*
 * Destroy node.
 */
bool linked_list_node_destroy(linked_list_node_type *node, va_list arg_list);

/*
 * Insert node to list.
 */

// insert after a node
bool linked_list_insert(linked_list_type *list, linked_list_node_type *index_node, linked_list_node_type *node);
// insert before a node
bool linked_list_insert_before(linked_list_type *list, linked_list_node_type *index_node, linked_list_node_type *node);
// insert at the tail of the list
bool linked_list_insert_back(linked_list_type *list, void *data);

/*
 * Search node in list.
 */
linked_list_node_type *linked_list_search(linked_list_type *list, void *data, bool (*equal)(void *, void *));

/*
 * Delete node in list.
 */
bool linked_list_delete(linked_list_type *list, linked_list_node_type *node, ...);

/*
 * String for debug.
 */
char *get_linked_list_debug_str(linked_list_type *list, ...);
char *linked_list_debug_str(linked_list_type *list, va_list arg_list);

/*
 * Copy linked list.
 */
linked_list_type *linked_list_copier(linked_list_type *list, va_list arg_list);
DECLARE_COPY(linked_list)

/*
 * Copy linked list's node.
 */
linked_list_node_type *linked_list_node_copier(linked_list_node_type *node, va_list arg_list);
#endif
