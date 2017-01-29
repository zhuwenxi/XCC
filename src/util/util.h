#ifndef __UTIL_H__
#define __UTIL_H__

#include "regexp/regexp.h"

//
// Convert grammar token to string.
//
#define TO_STRING(token) #token

//
// DBCODE macro: for debug purpose.
//
#ifdef DEBUG
#define DBCODE(flag, code)\
do{\
\
	if(flag) {\
		code;\
	}\
} while(0)
#else
#define DBCODE(flag, code)
#endif





//
// Boolean type: true & false.
//
typedef int bool;
#define true 1
#define false 0




//
// Bi-direction linked list.
//
typedef struct linked_list_node
{
	void *data;
	struct linked_list_node *next;
	struct linked_list_node *prev;
} linked_list_node_t;

typedef struct 
{
	linked_list_node_t *head;
	linked_list_node_t *tail;
} linked_list_t;

linked_list_node_t *
linked_list_first_item(linked_list_t *list);

linked_list_node_t *
linked_list_last_item(linked_list_t *list);

linked_list_node_t *
linked_list_limit(linked_list_t *list);

linked_list_node_t *
linked_list_next(linked_list_node_t *node);

linked_list_node_t *
linked_list_prev(linked_list_node_t *node);

linked_list_t *
linked_list_create();

void
linked_list_destroy(linked_list_t *list, void (*node_destructor)());

bool 
linked_list_append(linked_list_t *list, linked_list_node_t *node);

bool
linked_list_delete(linked_list_t *list, linked_list_node_t *node);


#define LL_NODE_DATA(node, type) ((type *)(node->data)) 


#endif
