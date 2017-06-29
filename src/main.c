#include "util/logger.h"
#include "util/string_buffer.h"
#include "util/linked_list.h"
#include "util/stack.h"
#include "util/hash_table.h"

#include <stdlib.h>
#include <stdarg.h>

static char *
str_to_str(void *data, va_list arg_list)
{
	string_buffer ret_str = string_buffer_create();
	string_buffer_append(&ret_str, "\"");

	char **str_ptr = (char **)data;
	string_buffer_append(&ret_str, *str_ptr);

	string_buffer_append(&ret_str, "\"");

	return ret_str;
}

static char *
int_to_str(void *data, va_list arg_list)
{
	string_buffer ret_str = string_buffer_create();
	string_buffer_append(&ret_str, "12306");

	return ret_str;
}

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");

	static char *keys[] = {"Abby", "Bob", "Cathy", "Douglas", "Emma"};
	static int values[] = {1, 2, 3, 4, 5};
	int item_num = 5;

	hash_table_type *hash_table = hash_table_create(string_hash);

	int i;
	for (i = 0; i < item_num; i ++)
	{
		hash_table_insert(hash_table, &keys[i], &values[i]);
	}

	printf("%s\n", get_hash_table_debug_str(hash_table, str_to_str, int_to_str, NULL));

	hash_table_destroy(hash_table, NULL);
}
