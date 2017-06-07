#include <stdlib.h>
#include <assert.h>

#include "util/array_list.h"
#include "util/logger.h"
#include "util/string_buffer.h"




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
array_list_destroy(array_list_type *list, ...)
{
	if (list == NULL)
	{
		return FALSE;
	}

	va_list ap;
	va_start(ap, list);
	array_list_deconstructor(list, ap);
	va_end(ap);

	return TRUE;
}

// bool
// array_list_destroy_diy(array_list_type *list, void (*data_deconstructor)(void *))
// {
// 	int i;
// 	for (i = 0; i < list->length; i++)
// 	{
// 		array_list_node_destroy(list->content[i], data_deconstructor);
// 	}

// 	free(list->content);
// 	free(list);
// 	return TRUE;
	
// }

bool
array_list_deconstructor(array_list_type *list, va_list arg_list)
{
	if (list == NULL)
	{
		return FALSE;
	}
	
	int i;
	for (i = 0; i < list->length; i++)
	{
		void *node = list->content[i];
		if (node != NULL)
		{
			va_list tmp;
			va_copy(tmp, arg_list);
			array_list_node_destroy(node, tmp);
			va_end(tmp);
		}
	}

	// destroy list itself
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
array_list_node_destroy(array_list_node_type *node, va_list arg_list)
{
	// get sub-deconstructor from "arg_list"
	bool (*sub_deconstructor)(void *, va_list);
	sub_deconstructor = va_arg(arg_list, bool (*)(void *, va_list));

	if (sub_deconstructor != NULL)
	{
		sub_deconstructor(node->data, arg_list);
	}

	free(node);

	return TRUE;
}

bool
array_list_append(array_list_type *list, void *data)
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

static int 
_negtive_index_to_positive(array_list_type *list, int index)
{
	assert(list != NULL);

	return index >= 0 ? index : list->length + index;
}

void *
array_list_get(array_list_type *list, int index)
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

void 
array_list_set(array_list_type *list, int index, void *data)
{
	assert(list != NULL);

	index = _negtive_index_to_positive(list, index);

	assert(index >= 0 && index < list->length);

	list->content[index]->data = data;
}

char *
get_array_list_debug_str(array_list_type *list, ...)
{
	va_list arg_list;
	va_start(arg_list, list);

	// int i;
	// for (i = 0; i < list->length; i ++)
	// {
	// 	char *item_str;

	// 	if (data_to_str != NULL)
	// 	{
	// 		item_str = data_to_str(array_list_get(list, i));
	// 	}
	// 	else
	// 	{
	// 		item_str = array_list_get(list, i);
	// 	}

	// 	string_buffer_append(&debug_str, item_str);

	// 	if (i != list->length - 1)
	// 		string_buffer_append(&debug_str, ", ");
	// }
	char* debug_str = array_list_debug_str(list, arg_list);

	va_end(arg_list);

	
	return debug_str;
}

char *
array_list_debug_str(array_list_type *list, va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, "[");

	va_list arg_list_copy;
	va_copy(arg_list_copy, arg_list);

	char *(*sub_debug_str)(void *, va_list);
	sub_debug_str = va_arg(arg_list_copy, char *(*)(void *, va_list));

	int i;
	for (i = 0; i < list->length; i++)
	{
		if (sub_debug_str != NULL)
		{
			char *item_str = sub_debug_str(list->content[i]->data, arg_list_copy);
			string_buffer_append(&debug_str, item_str);
			string_buffer_destroy(item_str);
		}
		else
		{
			string_buffer_append(&debug_str, list->content[i]->data);
		}

		if (i != list->length - 1)
			string_buffer_append(&debug_str, ", ");
	}

	

	va_end(arg_list_copy);

	string_buffer_append(&debug_str, "]");

	return debug_str;

}