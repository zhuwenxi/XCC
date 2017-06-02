#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "array_list.h"
#include "stddefs.h"

typedef struct
{
	array_list_type *buckets;
	int (*hash)(void *data);
} hash_table_type;

hash_table_type *hash_table_create(int (*hash)(void *));

bool hash_table_destroy(hash_table_type *table, void (*data_destroyer)());

bool hash_table_update(hash_table_type *table, void *key, void *value);

void *hash_table_search(hash_table_type *table, void *key);

bool hash_table_delete(hash_table_type *table, void *key);

#endif