#include "util/array_list_test.h"
#include "util/array_list.h"
#include "stddefs.h"
#include "test.h"
#include "util/util.h"
#include "util/logger.h"

#include <stddef.h>
#include <stdlib.h>

bool
array_list_create_destroy_test()
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

	array_list_destroy(list, NULL);

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

	array_list_destroy(list, NULL);

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

	array_list_destroy(list, NULL);

	return TRUE;
}

static bool
int_destroyer(void *int_data, va_list arg_list)
{
	free(int_data);
	return TRUE;
}

bool
array_list_multi_dim_test()
{
	/*
	 * outer_list = [inner_list_1, inner_list_2, inner_list_3]
	 */
	array_list_type *outer_list = array_list_create();
	array_list_type *inner_list_1 = array_list_create();
	array_list_type *inner_list_2 = array_list_create();
	array_list_type *inner_list_3 = array_list_create();

	array_list_append(outer_list, inner_list_1);
	array_list_append(outer_list, inner_list_2);
	array_list_append(outer_list, inner_list_3);

	
	/* inner_list_1 = [0, 1, 2]
	 * inner_list_2 = [3, 4, 5]
	 * inner_list_3 = [6, 7, 8]
	 */ 
	 
	// init data
	#define data_num 9
	int *p_data[data_num];
	int i;
	for (i = 0; i < data_num; i++)
	{
		p_data[i] = (int *)malloc(sizeof(int));
		*p_data[i] = i;
	}

	for (i = 0; i < 3; i++)
	{
		array_list_append(inner_list_1, p_data[i]);
	}

	for (i = 3; i < 6; i++)
	{
		array_list_append(inner_list_2, p_data[i]);
	}

	for (i = 6; i < 9; i++)
	{
		array_list_append(inner_list_3, p_data[i]);
	}

	// destroy the outerlist
	array_list_destroy(outer_list, array_list_deconstructor, int_destroyer, NULL);
	
	return TRUE;
}
