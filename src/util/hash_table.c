#include "util/hash_table.h"
#include "util/linked_list.h"
#include "util/util.h"
#include "util/logger.h"
#include "util/string_buffer.h"
#include "opts.h"

#include <stdlib.h>
#include <assert.h>
#include <math.h>

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
_hash_table_destroy_impl(hash_table_type *table, ...)
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

// bool
// hash_table_deconstructor(hash_table_type *table, va_list arg_list)
// {
// 	if (table == NULL)
// 	{
// 		return FALSE;
// 	}
	
// 	array_list_type *list = table->buckets;
	
// 	// destroy "array_list_type *" table->buckets
// 	int i;
// 	for (i = 0; i < list->capacity; i++)
// 	{
// 		void *node = list->content[i];
// 		if (node != NULL)
// 		{
// 			va_list tmp;
// 			va_copy(tmp, arg_list);

// 			// destroy "linked_list type *" node->data
// 			linked_list_type *linked_list = ((array_list_node_type *)node)->data;
// 			linked_list_node_type *linked_list_node = linked_list->head;
// 			while (linked_list_node != NULL)
// 			{
// 				linked_list_node_type *next_node = linked_list_node->next;

// 				va_list arg_list_copy_for_linked_list;
// 				va_copy(arg_list_copy_for_linked_list, tmp);
// 				hash_table_element_deconstructor(linked_list_node->data, arg_list_copy_for_linked_list);
// 				va_end(arg_list_copy_for_linked_list);

// 				free(linked_list_node);
// 				linked_list_node = next_node;

// 			}
// 			va_end(tmp);

// 			free(linked_list);
// 			free(node);
// 		}
// 	}

// 	// destroy list itself
// 	free(list->content);
// 	free(list);

// 	free(table);

// 	return TRUE;
// }

bool
hash_table_deconstructor(hash_table_type *table, va_list arg_list)
{
	if (table == NULL)
	{
		return FALSE;
	}
	
	array_list_type *list = table->buckets;

	array_list_deconstructor(list, arg_list);

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
hash_table_element_deconstructor(hash_table_element_type *element, va_list arg_list)
{
	if (element == NULL)
	{
		return FALSE;
	}

	bool (*key_deconstructor)(void *, va_list);
	key_deconstructor = va_arg(arg_list, void *);

	if (key_deconstructor == NULL)
	{
		free(element);
		return TRUE;
	}
	key_deconstructor(element->key, arg_list);

	bool (*value_deconstructor)(void *, va_list);
	value_deconstructor = va_arg(arg_list, void *);

	if (value_deconstructor == NULL)
	{
		free(element);
		return TRUE;
	}
	value_deconstructor(element->value, arg_list);


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
_search_element(hash_table_type *table, void *key, bool (*equal)(void *, void *, va_list), va_list arg_list)
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

	linked_list_node_type *linked_list_node = linked_list_search(bucket, key, hash_table_element_comparator, equal, arg_list);

	if (linked_list_node == NULL)
	{
		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "linked_list_node is NULL");
		return NULL;
	}

	return linked_list_node;
}

void *
hash_table_searcher(hash_table_type *table, void *key, bool (*comparator)(void *, void *, va_list), va_list arg_list)
{
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "hash_table_search");
	
	assert(table != NULL && key != NULL);

	linked_list_node_type *linked_list_node = _search_element(table, key, comparator, arg_list);
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

array_list_type *
hash_table_all_searcher(hash_table_type *table, void *key, bool(*comparator)(void *, void *, va_list), va_list arg_list)
{
	array_list_type *ret = array_list_create();

	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "hash_table_search");

	assert(table != NULL && key != NULL);

	linked_list_node_type *linked_list_node = _search_element(table, key, comparator, arg_list);
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_SEARCH_LOG_ENABLE, "node: 0x%x", linked_list_node);

	/*if (linked_list_node != NULL)
	{
		return TYPE_CAST(linked_list_node->data, hash_table_element_type *)->value;
	}*/
	while (linked_list_node)
	{
		hash_table_element_type *elem = linked_list_node->data;
		void *k = elem->key;
		
		if (comparator(k, key, NULL))
		{
			void *v = elem->value;
			array_list_append(ret, v);
		}

		linked_list_node = linked_list_node->next;
	}
	
	return ret;
}

bool
hash_table_deletor(hash_table_type *table, void *key, bool (*equal)(void *, void *, va_list), va_list arg_list)
{
	assert(table != NULL && key != NULL);

	linked_list_node_type *linked_list_node = _search_element(table, key, equal, arg_list);
	LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_DELETE_LOG_ENABLE, "linked_list_node: 0x%x", linked_list_node);

	if (linked_list_node != NULL)
	{
		int slot_index = _select_slot(table, key);

		array_list_node_type *slot = table->buckets->content[slot_index];
		linked_list_type *bucket = (linked_list_type *)(slot->data);

		LOG(HASH_TABLE_LOG_ENABLE && HASH_TABLE_DELETE_LOG_ENABLE, "find the bucket: 0x%x", bucket);
		// linked_list_delete(bucket, linked_list_node, NULL);
		linked_list_type *list = bucket;
		linked_list_node_type *node = linked_list_node;

		linked_list_node_type *next_node = node->next;
		linked_list_node_type *prev_node = node->prev;
	

		/*
		 * prev_node->next = next_node;
		 */	
		if (prev_node == NULL)
		{
			// node == list->head	
			assert(list->head == node);
		
			list->head = next_node;
		}
		else
		{
			assert(list->head != node);

			prev_node->next = node->next;
		}

		/*
		 * next_node->prev = prev_node;
		 */
		if (next_node == NULL)
		{
			assert(list->tail == node);

			list->tail = prev_node;
		}
		else
		{
			assert(list->tail != node);

			next_node->prev = prev_node;
		}	

		
		hash_table_element_deconstructor(node->data, arg_list);
		free(node);
		
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

	linked_list_node_type *node = list->head;
	while (node != NULL)
	{
		va_list arg_list_copy;
		va_copy(arg_list_copy, arg_list);

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
		va_end(arg_list_copy);
		
		node = next_node;
	}

	

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

	string_buffer_append(&debug_str, "{ ");

	va_list arg_list_copy;
	va_copy(arg_list_copy, arg_list);

	// print as array_list
	int i;
	array_list_type *list = table->buckets;
	for (i = 0; i < list->capacity; i++)
	{
		if (list->content[i] == NULL) continue;
		
		string_buffer item_str = _print_as_linked_list(list->content[i]->data, hash_table_element_str, arg_list_copy);

		if (item_str != NULL)
		{
			string_buffer_append(&debug_str, item_str);
			string_buffer_destroy(item_str);
		}
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
	get_key_str = va_arg(arg_list, void *);

	char *key_str = get_key_str(table_element->key, arg_list);
	// prapre value's debug str
	char *(*get_value_str)(void *, va_list);
	get_value_str = va_arg(arg_list, void *);

	char *value_str = get_value_str(table_element->value, arg_list);

	string_buffer_append(&debug_str, key_str);
	string_buffer_append(&debug_str, ": ");
	string_buffer_append(&debug_str, value_str);

	string_buffer_destroy(key_str);
	string_buffer_destroy(value_str);

	return debug_str;
}

bool
hash_table_element_comparator(void *e1, void *e2, va_list arg_list)
{
	bool (*sub_comparator)(void *, void *, va_list) = va_arg(arg_list, void *);

	hash_table_element_type *element1;
	element1 = TYPE_CAST(e1, hash_table_element_type *);

	return sub_comparator(element1->key, e2, arg_list);
}

void
hash_table_traverse(hash_table_type *table, void (*visitor)(void *key, void *value, void *context), void *context)
{
	assert(table);

	if (!visitor) return;
	
	array_list_type *buckets = table->buckets;

	int i;
	for (i = 0; i < buckets->capacity; ++i) {
		array_list_node_type *bucket = buckets->content[i];

		if (bucket && bucket->data) {
			linked_list_node_type *ll_node = TYPE_CAST(bucket->data, linked_list_type*)->head;

			while (ll_node) {
				hash_table_element_type *ele = ll_node->data;

				if (ele != NULL) {
					assert(ele->key && ele->value);
					visitor(ele->key, ele->value, context);
				}
				ll_node = ll_node->next;
			}
		}
	}
}



/*
 * common hash function
 */
int
int_hash(void *int_val)
{
	return *((int *)int_val);
}

int
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


