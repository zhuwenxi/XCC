#include "util/hash_table.h"
#include "util/hash_table_test.h"
#include "util/linked_list.h"
#include "util/logger.h"
#include "util/util.h"
#include "test.h"

#include <string.h>

bool
hash_table_create_destroy_test()
{
	hash_table_type *hash_table = hash_table_create(string_hash);
	hash_table_destroy(hash_table, NULL);

	return TRUE;
}

static char *keys[] = {"Abby", "Bob", "Cathy", "Douglas", "Emma"};
static int values[] = {1, 2, 3, 4, 5};
int item_num = 5;

bool
hash_table_insert_test()
{
	hash_table_type *hash_table = hash_table_create(string_hash);

	int i;
	for (i = 0; i < item_num; i ++)
	{
		hash_table_insert(hash_table, &keys[i], &values[i]);
	}

	hash_table_destroy(hash_table, NULL);

	return TRUE;
}

static bool
str_equal(void *node_data, void *key)
{
	hash_table_element_type *element = node_data;

	return strcmp(*TYPE_CAST(element->key, char **), *TYPE_CAST(key, char **)) == 0;
}

bool
hash_table_search_test()
{
	hash_table_type *hash_table = hash_table_create(string_hash);

	int i;
	for (i = 0; i < item_num; i ++)
	{
		hash_table_insert(hash_table, &keys[i], &values[i]);
	}

	for (i = 0; i < item_num; i ++)
	{	
		int *val_ptr = hash_table_search(hash_table, &keys[i], str_equal);

		if (!EXPECT_POINTER_NOT_EQUAL(val_ptr, NULL))
		{
			return FALSE;
		}

		if (!EXPECT_EQUAL(*val_ptr, values[i]))
		{
			return FALSE;
		}
	}

	hash_table_destroy(hash_table, NULL);

	return TRUE;
}

bool
hash_table_delete_test()
{
	hash_table_type *hash_table = hash_table_create(string_hash);
	int i;
	for (i = 0; i < item_num; i ++)
	{
		hash_table_insert(hash_table, &keys[i], &values[i]);
	}
	
	for (i = 0; i < item_num; i ++)
	{
		bool ret = hash_table_delete(hash_table, &keys[i], str_equal, NULL);
		if (ret == FALSE)
		{
			return FALSE;
		}	
	}

	hash_table_destroy(hash_table, NULL);
	return TRUE;
}