#ifndef __ARRAY_LIST_H__
#define __ARRAY_LIST_H__




#include <stddef.h>
#include <stdarg.h>

#include "stddefs.h"
#include "util.h"

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
bool array_list_destroy(array_list_type *list, ...);

/*
 * De-constructor.
 */
bool array_list_deconstructor(array_list_type *list, va_list arg_list);

/*
 * Create array list node.
 */
array_list_node_type *array_list_node_create();

/*
 * Destory array list node.
 */
bool array_list_node_destroy(array_list_node_type *node, va_list arg_list);

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
 * Search data in array list.
 */
array_list_node_type *array_list_searcher(array_list_type *list, void *data, bool (*equal)(void *, void *, va_list), va_list arg_list);
DECLARE_SEARCH(array_list_type, array_list_node_type, array_list)

/*
 * Resize the internal dynamic array
 */
bool array_list_resize(array_list_type *list, int new_size);

/*
 * Adjust array list length
 */
bool array_list_adjust_length(array_list_type *list, int new_length);

/*
 * String for debug.
 */
char *get_array_list_debug_str(array_list_type *list, ...);

char *array_list_debug_str(array_list_type *list, va_list arg_list);

/*
 * Copy array lis.
 */
array_list_type *array_list_copier(array_list_type *list, va_list arg_list);
DECLARE_COPY(array_list)

/*
 * Copy array list's node.
 */
array_list_node_type *array_list_node_copier(array_list_node_type *node, va_list arg_list);
#endif