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
hash_table_destroy(hash_table_type *table, ...)
{
	va_list arg_list;
	va_start(arg_list, table);

	array_list_destroy(table->buckets, arg_list);
	free(table);

	va_end(arg_list);

	return TRUE;
}

