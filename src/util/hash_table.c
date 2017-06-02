#include "util/hash_table.h"

#include <stdlib.h>

hash_table_type *
hash_table_create(int (*hash)(void *))
{
	hash_table_type *table = (hash_table_type *)malloc(sizeof(hash_table_type));

	table->buckets = array_list_create();
	table->hash = hash;

	return table;
}

bool 
hash_table_destroy(hash_table_type *table, void (*data_destroyer)())
{
	array_list_destroy(table->buckets, data_destroyer);
	free(table);

	return TRUE;
}

