#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "array_list.h"
#include "stddefs.h"

#include <stdarg.h>
#include <string.h>
#include <math.h>

typedef struct
{
	array_list_type *buckets;
	int (*hash)(void *data);
} hash_table_type;

typedef struct
{
	void *key;
	void *value;
} hash_table_element_type;

hash_table_type *hash_table_create(int (*hash)(void *));

bool _hash_table_destroy_impl(hash_table_type *table, ...);

// bool hash_table_destroy(hash_table_type *table, ...);
#define hash_table_destroy(table, ...) \
	_hash_table_destroy_impl(table, linked_list_deconstructor, hash_table_element_deconstructor, __VA_ARGS__);

bool hash_table_deconstructor(hash_table_type *table, va_list arg_list);

hash_table_element_type *hash_table_element_create(void *key, void *value);

bool hash_table_element_deconstructor(hash_table_element_type *element, va_list arg_list);

bool hash_table_element_comparator(void *e1, void *e2, va_list arg_list);

bool hash_table_insert(hash_table_type *table, void *key, void *value);

void *hash_table_searcher(hash_table_type *table, void *key, bool (*comparator)(void *, void *, va_list), va_list arg_list);
DECLARE_SEARCH(hash_table_type, void, hash_table)

array_list_type *hash_table_all_searcher(hash_table_type *table, void *key, bool(*comparator)(void *, void *, va_list), va_list arg_list);
DECLARE_ALL_SEARCH(hash_table_type, array_list_type, hash_table)
#define hash_table_search_all hash_table_all_searcher

bool hash_table_deletor(hash_table_type *table, void *key, bool (*equal)(void *, void *, va_list), va_list arg_list);
DECLARE_DELETE(hash_table_type, hash_table);

char *get_hash_table_debug_str(hash_table_type *table, ...);

char *hash_table_debug_str(hash_table_type *table, va_list arg_list);

char *hash_table_element_str(hash_table_element_type *table_element, va_list arg_list);
// We use division method to create hash function
#define HASH_TABLE_SLOT_NUMBER 701

void hash_table_traverse(hash_table_type *table, void (*visitor)(void *key, void *value, void *context), void *context);

/*
 * common hash function
 */
int int_hash(void *int_val);

int string_hash(void *string_val);

#endif