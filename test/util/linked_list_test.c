#include <stdio.h>

#include "util/linked_list.h"
#include "util/linked_list_test.h"
#include "stddefs.h"
#include "test.h" 
#include "util/logger.h"
#include "util/util.h"



bool linked_list_test()
{
	return linked_list_create_test() == TRUE;
}

bool linked_list_create_test()
{
	linked_list_type *list = linked_list_create();
	
	if (list == NULL || list->head != NULL)
	{
		return FALSE;
	} 
	else 
	{
		return TRUE;
	}
}

bool linked_list_insert_back_test()
{
	linked_list_type *list = linked_list_create();
	
	int element[] = {2, 3, 5, 7, 11};
	int element_num = 5;
	int i;
	for (i = 0; i < element_num; i ++)
	{
		linked_list_insert_back(list, &element[i]);
	}

	linked_list_node_type *current_node = NULL;

	for (i = 0; i < element_num; i++)
	{
		if (current_node == NULL)
		{
			current_node = list->head;
		}	

		if(!EXPECT_EQUAL(*((int *)(current_node->data)), element[i]))
		{
			return FALSE;
		}

		current_node = current_node->next;
	}
	
	return TRUE;
}

static bool int_equal(void *a, void *b)
{
	int a_data = *TYPE_CAST(a, int*);
	int b_data = *TYPE_CAST(b, int*);
	
	return a_data == b_data;
}

bool linked_list_search_test()
{
	linked_list_type *list = linked_list_create();
	
	int element[] = {2, 3, 5, 7, 11};
	int element_num = 5;
	int i;
	for (i = 0; i < element_num; i ++)
	{
		linked_list_insert_back(list, &element[i]);
	}
	
	for (i = 0; i < element_num; i++)
	{
		linked_list_node_type *node = linked_list_search(list, &element[i], int_equal);
		int node_data = *TYPE_CAST(node->data, int *);
		if(!EXPECT_EQUAL(node_data, element[i]))
		{
			return FALSE;
		}

	}

	return TRUE;
}





