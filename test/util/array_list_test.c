#include "util/array_list_test.h"
#include "util/array_list.h"
#include "stddefs.h"
#include "test.h"
#include "util/util.h"
#include "util/logger.h"

#include <stddef.h>

bool
array_list_create_test()
{
	array_list_type *list = array_list_create();
	return TRUE;
}

bool
array_list_destroy_test()
{
	array_list_type *list = array_list_create();
	array_list_destroy(list, NULL);

	return TRUE;
}

bool
array_list_append_test()
{
	array_list_type *list = array_list_create();

	int len = 30;
	int data[len];
	int i;

	for (i = 0; i < len; i++)
	{
		data[i] = i;
		array_list_append(list, &data[i]);
		if (!EXPECT_EQUAL(GET_DATA(list->content[i], int), i)) return FALSE;
	}

	if (!EXPECT_EQUAL(GET_DATA(list->content[0], int), 0))
	{
		return FALSE;
	}
	else if (!EXPECT_EQUAL((int)list->length, len))
	{
		return FALSE;
	}

	return TRUE;
}

bool
array_list_get_test()
{
	array_list_type *list = array_list_create();

	int len = 30;
	int data[len];
	int i;

	for (i = 0; i < len; i++)
	{
		data[i] = i;
		array_list_append(list, &data[i]);
		if (!EXPECT_EQUAL(*(int *)array_list_get(list, i), i)) return FALSE;
	}

	return TRUE;
}

bool
array_list_set_test()
{
	array_list_type *list = array_list_create();

	int len = 30;
	int data[len];
	int i;

	for (i = 0; i < len; i++)
	{
		data[i] = i;
		array_list_append(list, &data[i]);
	}

	int default_data = 110;
	for (i = 0; i < len; i++)
	{
		array_list_set(list, i, &default_data);
	}

	for (i = 0; i < len; i++)
	{
		if (!EXPECT_EQUAL(*(int *)array_list_get(list, i), default_data)) return FALSE;
	}

	return TRUE;
}
