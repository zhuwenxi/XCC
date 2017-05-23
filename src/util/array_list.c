#include <stdlib.h>

#include "util/array_list.h"
#include "util/logger.h"




array_list_type *
array_list_create()
{
	array_list_type *list = (array_list_type *)malloc(sizeof(array_list_type));
	list->content = NULL;
	list->length = 0;
	list->capacity = 16;
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

