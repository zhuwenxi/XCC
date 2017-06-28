#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "array_list.h"
#include "stddefs.h"

#include <stdarg.h>

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

bool hash_table_destroy(hash_table_type *table, void (*hash_table_deconstructor)(hash_table_type *));

// bool hash_table_deconstructor(hash_table_type *table, va_list arg_list);

hash_table_element_type *hash_table_element_create(void *key, void *value);

bool hash_table_element_destroy(hash_table_element_type *element, void (*hash_table_element_deconstructor)(hash_table_element_type *));

bool hash_table_update(hash_table_type *table, void *key, void *value);

void *hash_table_search(hash_table_type *table, void *key, bool (*equal)(void *, void *));

bool hash_table_delete(hash_table_type *table, void *key, bool (*equal)(void *, void *));

// We use division method to create hash function
#define HASH_TABLE_SLOT_NUMBER 701

#endif