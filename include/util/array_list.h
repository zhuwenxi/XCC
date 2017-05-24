#ifndef __ARRAY_LIST_H__
#define __ARRAY_LIST_H__




#include "stddefs.h"

typedef struct array_list_node
{
	void *data;
} array_list_node_type;

typedef struct array_list
{
	size_t length;
	size_t capacity;
	array_list_node_type **content;
} array_list_type;

/*
 * Create array list.
 */
array_list_type *array_list_create();

/*
 * Destroy array list.
 */
bool array_list_destroy(array_list_type *list, void (*data_deconstructor)(void *));

/*
 * Create array list node.
 */
array_list_node_type *array_list_node_create();

/*
 * Destory array list node.
 */
bool array_list_node_destroy(array_list_node_type *node, void (*data_deconstructor)(void *));

/*
 * Append node to array list.
 */
bool array_list_append(array_list_type *list, void *data);

/*
 * Get data at the i-index node
 */
void *array_list_get(array_list_type *list,int index);

/*
 * Set data at the i-index node
 */
void array_list_set(array_list_type *list, int index, void *data);

/*
 * String for debug.
 */
char *array_list_debug_str(array_list_type *list, char * (*data_to_str)(void *data));

#endif