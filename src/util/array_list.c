#include <stdlib.h>
#include <assert.h>

#include "util/array_list.h"
#include "util/logger.h"




array_list_type *
array_list_create()
{
	array_list_type *list = (array_list_type *)malloc(sizeof(array_list_type));
	
	list->length = 0;
	list->capacity = 16;

	list->content = (array_list_node_type **)malloc(sizeof(array_list_node_type *) * list->capacity);
	return list;
}

bool
array_list_destroy(array_list_type *list, void (*data_deconstructor)(void *))
{
	int i;
	for (i = 0; i < list->length; i++)
	{
		array_list_node_destroy(list->content[i], data_deconstructor);
	}

	free(list->content);
	free(list);
	return TRUE;
	
}

array_list_node_type *
array_list_node_create()
{
	array_list_node_type *node = (array_list_node_type *)malloc(sizeof(array_list_node_type));
	node->data = NULL;

	return node;
}

bool
array_list_node_destroy(array_list_node_type *node, void (*data_deconstructor)(void *))
{
	if (data_deconstructor != NULL)
	{
		data_deconstructor(node->data);
	}

	free(node);

	return TRUE;
}

bool array_list_append(array_list_type *list, void *data)
{
	assert(list != NULL);

	list->length += 1;

	// if list's capacity not enough for new element, increase the capacity by 50%.
	if (list->length > list->capacity)
	{
		list->capacity *= 1.5;
		list->content = (array_list_node_type **)realloc(list->content, sizeof(array_list_node_type *) * list->capacity);
	}

	// Create append node and set data.
	array_list_node_type *append_node = array_list_node_create();
	append_node->data = data;
	list->content[list->length - 1] = append_node;

	return TRUE;
}

static int _negtive_index_to_positive(array_list_type *list, int index)
{
	assert(list != NULL);

	return index >= 0 ? index : list->length + index;
}

void *array_list_get(array_list_type *list, int index)
{
	assert(list != NULL);

	index = _negtive_index_to_positive(list, index);

	if (index < 0 || index > list->length - 1)
	{
		return NULL;
	}
	else
	{
		return list->content[index]->data;
	}
}

void array_list_set(array_list_type *list, int index, void *data)
{
	assert(list != NULL);

	index = _negtive_index_to_positive(list, index);

	assert(index >= 0 && index < list->length);

	list->content[index]->data = data;
}

char *array_list_debug_str(array_list_type *list, char * (*data_to_str)(void *data))
{

}