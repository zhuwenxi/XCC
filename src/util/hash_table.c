#include "util/hash_table.h"
#include "util/linked_list.h"
#include "util/util.h"

#include <stdlib.h>
#include <assert.h>

hash_table_type *
hash_table_create(int (*hash)(void *))
{
	hash_table_type *table = (hash_table_type *)malloc(sizeof(hash_table_type));

	table->buckets = array_list_create();
	array_list_resize(table->buckets, HASH_TABLE_SLOT_NUMBER);

	table->hash = hash;
	return table;
}

bool 
hash_table_destroy(hash_table_type *table, void (*hash_table_deconstructor)(hash_table_type *))
{
	if (hash_table_deconstructor == NULL)
	{
		return FALSE;
	}

	hash_table_deconstructor(table);
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
hash_table_update(hash_table_type *table, void *key, void *value)
{
	assert(table != NULL);

	if (key == NULL)
	{
		return FALSE;
	}

	int slot_index = _select_slot(table, key);

	array_list_node_type *slot = table->buckets->content[slot_index];
	if (slot == NULL)
	{
		slot = array_list_node_create();
		table->buckets->content[slot_index] = slot;
	}

	linked_list_type *bucket = slot->data;
	if (bucket == NULL)
	{
		bucket = linked_list_create();
	}

	linked_list_insert_back(bucket, hash_table_element_create(key, value));

	return TRUE;
}

void *
_search_element(hash_table_type *table, void *key, bool (*equal)(void *, void *))
{
	int slot_index = _select_slot(table, key);

	array_list_node_type *slot = table->buckets->content[slot_index];
	if (slot == NULL)
	{
		return NULL;
	}

	linked_list_type *bucket = slot->data;
	if (bucket == NULL)
	{
		return NULL;
	}

	linked_list_node_type *linked_list_node = linked_list_search(bucket, key, equal);

	if (linked_list_node == NULL)
	{
		return NULL;
	}

	return linked_list_node;
}

void *
hash_table_search(hash_table_type *table, void *key, bool (*equal)(void *, void *))
{
	assert(table != NULL && key != NULL);

	linked_list_node_type *linked_list_node = _search_element(table, key, equal);

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

	if (linked_list_node != NULL)
	{
		int slot_index = _select_slot(table, key);

		array_list_node_type *slot = table->buckets->content[slot_index];
		linked_list_type *bucket = (linked_list_type *)slot->data;
		linked_list_delete(bucket, linked_list_node);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


