#include "util/hash_table.h"
#include "util/linked_list.h"
#include "util/util.h"
#include "util/logger.h"
#include "util/string_buffer.h"
#include "opts.h"

#include <stdlib.h>
#include <assert.h>

hash_table_type *
hash_table_create(int (*hash)(void *))
{
	assert(hash != NULL);

	hash_table_type *table = (hash_table_type *)malloc(sizeof(hash_table_type));

	table->buckets = array_list_create();
	array_list_resize(table->buckets, HASH_TABLE_SLOT_NUMBER);

	table->hash = hash;
	return table;
}

bool 
hash_table_destroy(hash_table_type *table, ...)
{
	if (table == NULL)
	{
		return FALSE;
	}

	va_list ap;
	va_start(ap, table);
	hash_table_deconstructor(table, ap);
	va_end(ap);

	return TRUE;
}

bool
hash_table_deconstructor(hash_table_type *table, va_list arg_list)
{
	if (table == NULL)
	{
		return FALSE;
	}

	array_list_deconstructor(table->buckets, arg_list);
	free(table);

	return TRUE;
}

hash_table_element_type *
hash_table_element_create(void *key, void *value)
{
	hash_table_element_type *element = (hash_table_element_type *)malloc(sizeof(hash_table_element_type));

	element->key = key;
	element->value = value;

	return element;
}

bool
hash_table_element_destroy(hash_table_element_type *element, void (*hash_table_element_deconstructor)(hash_table_element_type *))
{
	if (hash_table_element_deconstructor == NULL)
	{
		return FALSE;
	}

	hash_table_element_deconstructor(element);
	free(element);

	return TRUE;
}

// select slot by hash code
static int
_select_slot(hash_table_type *table, void *key)
{
	return table->hash(key) % HASH_TABLE_SLOT_NUMBER;
}

bool
hash_table_insert(hash_table_type *table, void *key, void *value)
{
	assert(table != NULL);

	if (key == NULL)
	{
		return FALSE;
	}

	int slot_index = _select_slot(table, key);
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_INSERT_LOG_ENABLE, "slot_index: %d", slot_index);

	array_list_node_type *slot = table->buckets->content[slot_index];

	if (slot == NULL)
	{
		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_INSERT_LOG_ENABLE, "slot is NULL");
		slot = array_list_node_create();
		table->buckets->content[slot_index] = slot;
	}

	linked_list_type *bucket = slot->data;
	if (bucket == NULL)
	{
		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_INSERT_LOG_ENABLE, "bucket is NULL");
		slot->data = linked_list_create();
	}

	linked_list_insert_back(slot->data, hash_table_element_create(key, value));
	
	return TRUE;
}

void *
_search_element(hash_table_type *table, void *key, bool (*equal)(void *, void *))
{
	int slot_index = _select_slot(table, key);
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "slot_index: %d", slot_index);

	array_list_node_type *slot = table->buckets->content[slot_index];
	if (slot == NULL)
	{
		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "slot is NULL");
		return NULL;
	}

	linked_list_type *bucket = slot->data;
	if (bucket == NULL)
	{
		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "bucket is NULL");
		return NULL;
	}

	linked_list_node_type *linked_list_node = linked_list_search(bucket, key, equal);

	if (linked_list_node == NULL)
	{
		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "linked_list_node is NULL");
		return NULL;
	}

	return linked_list_node;
}

void *
hash_table_search(hash_table_type *table, void *key, bool (*equal)(void *, void *))
{
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "hash_table_search");
	
	assert(table != NULL && key != NULL);

	linked_list_node_type *linked_list_node = _search_element(table, key, equal);
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "node: 0x%x", linked_list_node);

	if (linked_list_node != NULL)
	{
		return TYPE_CAST(linked_list_node->data, hash_table_element_type *)->value;
	}
	else
	{
		return NULL;
	}
}

bool
hash_table_delete(hash_table_type *table, void *key, bool (*equal)(void *, void *))
{
	assert(table != NULL && key != NULL);

	linked_list_node_type *linked_list_node = _search_element(table, key, equal);
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_DELETE_LOG_ENABLE, "linked_list_node: 0x%x", linked_list_node);

	if (linked_list_node != NULL)
	{
		int slot_index = _select_slot(table, key);

		array_list_node_type *slot = table->buckets->content[slot_index];
		linked_list_type *bucket = (linked_list_type *)(slot->data);

		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_DELETE_LOG_ENABLE, "find the bucket: 0x%x", bucket);
		linked_list_delete(bucket, linked_list_node, NULL);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

char *
get_hash_table_debug_str(hash_table_type *table, ...)
{
	va_list arg_list;
	va_start(arg_list, table);

	char* debug_str = hash_table_debug_str(table, arg_list);

	va_end(arg_list);

	return debug_str;
}

static char *
_print_as_linked_list(linked_list_type *list, char *(*sub_debug_str)(hash_table_element_type *, va_list), va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	va_list arg_list_copy;
	va_copy(arg_list_copy, arg_list);

	linked_list_node_type *node = list->head;
	while (node != NULL)
	{
		linked_list_node_type *next_node = node->next;

		char *item_str;
		if (sub_debug_str != NULL)
		{
			item_str = sub_debug_str(node->data, arg_list_copy);
			string_buffer_append(&debug_str, item_str);
			string_buffer_destroy(item_str);
		}
		else
		{
			item_str = (char *)node->data;
			string_buffer_append(&debug_str, item_str);
		}


		string_buffer_append(&debug_str, ", ");

		node = next_node;
	}

	va_end(arg_list_copy);

	return debug_str;
}

char *
hash_table_debug_str(hash_table_type *table, va_list arg_list)
{
	if (table == NULL)
	{
		return NULL;
	}

	string_buffer debug_str = string_buffer_create();

	string_buffer_append(&debug_str, "{");

	va_list arg_list_copy;
	va_copy(arg_list_copy, arg_list);

	// char *(*sub_debug_str)(void *, va_list);
	// sub_debug_str = va_arg(arg_list_copy, char *(*)(void *, va_list));

	// print as array_list
	int i;
	array_list_type *list = table->buckets;
	for (i = 0; i < list->capacity; i++)
	{
		if (list->content[i] == NULL) continue;
		
		// if (sub_debug_str != NULL)
		// {
		// pritn as linked_list
		// char *item_str = sub_debug_str(list->content[i]->data, arg_list_copy);
		string_buffer item_str = _print_as_linked_list(list->content[i]->data, hash_table_element_str, arg_list_copy);

		if (item_str != NULL)
		{
			string_buffer_append(&debug_str, item_str);
			string_buffer_destroy(item_str);
			// string_buffer_append(&debug_str, ",");
		}
		// }
		// else
		// {
		// 	if (list->content[i]->data != NULL)
		// 	{
		// 		string_buffer_append(&debug_str, list->content[i]->data);
		// 		string_buffer_append(&debug_str, ",");
		// 	}
		// }
	}

	va_end(arg_list_copy);

	string_buffer_append(&debug_str, "}");

	return debug_str;
}

char *hash_table_element_str(hash_table_element_type *table_element, va_list arg_list)
{
	string_buffer debug_str = string_buffer_create();

	// prepare key's debug str
	char *(*get_key_str)(void *, va_list);
	get_key_str = va_arg(arg_list, char *(*)(void *, va_list));

	char *key_str = get_key_str(table_element->key, arg_list);
	// prapre value's debug str
	char *(*get_value_str)(void *, va_list);
	get_value_str = va_arg(arg_list, char *(*)(void *, va_list));

	char *value_str = get_value_str(table_element->value, arg_list);

	string_buffer_append(&debug_str, key_str);
	string_buffer_append(&debug_str, ": ");
	string_buffer_append(&debug_str, value_str);

	string_buffer_destroy(key_str);
	string_buffer_destroy(value_str);

	return debug_str;
}


/*
 * common hash function
 */
inline int
int_hash(void *int_val)
{
	return *((int *)int_val);
}

inline int
string_hash(void *string_val)
{
	int hashcode = 0;
	char *string = *((char **)string_val);

	int i;
	for (i = 0; i < strlen(string); i++)
	{
		hashcode += string[i] * pow(10, i);
	}

	return hashcode;
}


