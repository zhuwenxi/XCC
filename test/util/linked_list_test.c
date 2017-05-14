#include <stdio.h>
#include <stdlib.h>

#include "util/linked_list.h"
#include "util/linked_list_test.h"
#include "stddefs.h"
#include "test.h" 
#include "util/logger.h"
#include "util/util.h"




/*
 * helper functions:
 */
static bool
int_equal(void *a, void *b)
{
	int a_data = *TYPE_CAST(a, int*);
	int b_data = *TYPE_CAST(b, int*);
	
	return a_data == b_data;
}

static void
destroy_int(void *data)
{
	int *int_pointer = TYPE_CAST(data, int*);	
	free(int_pointer);
}

/*
 * linked_list_create() tests.
 */
bool
linked_list_create_test()
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


/*
 * linked_list_destroy() tests.
 */
bool
linked_list_destroy_test()
{
	// prepare data.
	int *int_pointer[7];
	int i;
	for (i = 0; i < 7; i ++)
	{
		int_pointer[i] = (int *)malloc(sizeof(int));
	}

	// create linked_list and add data to it.
	linked_list_type *list = linked_list_create();
	for (i = 0; i < 7; i ++)
	{
		linked_list_insert_back(list, TYPE_CAST(int_pointer[i], void *));
	}

	// destroy the list
	linked_list_destroy(list, destroy_int);

	return TRUE;
}


/*
 * linked_list_insert() tests.
 */
bool
linked_list_insert_test()
{
	linked_list_type *list = linked_list_create();

	int element[] = {2, 3, 5, 7, 11};
	int element_num = 5;
	int i;
	for (i = 0; i < element_num; i ++)
	{
		linked_list_insert_back(list, &element[i]);
	}

	linked_list_node_type *node2 = linked_list_search(list, &element[2], int_equal);

	int new_data = 6;
	linked_list_node_type *new_node = linked_list_node_create();
	new_node->data = &new_data;

	linked_list_insert(list, node2, new_node);

	int new_element[] = {2, 3, 5, 6, 7, 11};
	linked_list_node_type *node = list->head;
	for (i = 0; i < 6; i++)
	{
		if (!EXPECT_EQUAL(*TYPE_CAST(node->data, int *), new_element[i]))
		{
			return FALSE;
		}
		node = node->next;	
	}

	linked_list_node_type *last_node = linked_list_search(list, &new_element[5], int_equal);
	
	int new_data_2 = 12;
	linked_list_node_type *old_node = new_node;
	new_node = linked_list_node_create();
	new_node->data = &new_data_2;
	
	linked_list_insert(list, last_node, new_node);

	int new_element_2[] = {2, 3, 5, 6, 7, 11, 12};
	node = list->head;
	for (i = 0; i < 7; i++)
	{
		if (!EXPECT_EQUAL(*TYPE_CAST(node->data, int *), new_element_2[i]))
		{
			return FALSE;
		}

		node = node->next;	
	}

	return TRUE;
}

/*
 * linked_list_insert_back() tests.
 */
bool
linked_list_insert_back_test()
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


/*
 * linked_list_search() tests.
 */
bool
linked_list_search_test()
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

/*
 * linked_list_delete() tests.
 */
bool
linked_list_delete_test()
{
	// Initialize the list.
	linked_list_type *list = linked_list_create();
	
	int element[] = {2, 3, 5, 7, 11};
	int element_num = 5;
	int i;

	int *int_p[5];
	for (i = 0; i < element_num; i++)
	{
		int_p[i] = (int *)malloc(sizeof(int));	
		*(int_p[i]) = element[i];
	}

	// int *int_p = (int *)malloc(sizeof(int) * element_num);

	for (i = 0; i < element_num; i ++)
	{
		linked_list_insert_back(list, int_p[i]);
	}

	// "target" node whose value is 5
	linked_list_node_type *target = linked_list_search(list, &element[2], int_equal);

	// delete the "target" node
	linked_list_delete(list, target, destroy_int);

	// "node" to iterate the list
	linked_list_node_type *node = list->head;

	int new_element[] = {2, 3, 7, 11};

	for (i = 0; i < 4; i++)	
	{
		if (!EXPECT_EQUAL(*TYPE_CAST(node->data, int *), new_element[i]))
		{
			return FALSE;
		}
		node = node->next;	
	}

	// delete the head && tail
	linked_list_delete(list, list->head, destroy_int);
	linked_list_delete(list, list->tail, destroy_int);

	int new_element_2[] = {3, 7};
	node = list->head;

	for (i = 0; i < 2; i++)	
	{
		if (!EXPECT_EQUAL(*TYPE_CAST(node->data, int *), new_element_2[i]))
		{
			return FALSE;
		}
		node = node->next;	
	}

	return TRUE;
}



