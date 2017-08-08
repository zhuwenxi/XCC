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
static void
destroy_int(void *data, va_list arg_list)
{
	int *int_pointer = TYPE_CAST(data, int*);	
	free(int_pointer);
}


/*
 * linked_list_create() and linked_list_destroy() tests.
 */
bool
linked_list_create_destroy_test()
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
	linked_list_destroy(list, destroy_int, NULL);

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

	linked_list_node_type *node2 = linked_list_search(list, &element[2], int_comparator, NULL);

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

	linked_list_node_type *last_node = linked_list_search(list, &new_element[5], int_comparator, NULL);
	
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

	linked_list_destroy(list, NULL);

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

	linked_list_destroy(list, NULL);
	
	return TRUE;
}

/*
 * linked_list_insert_before() tests.
 */
bool
linked_list_insert_before_test()
{
	linked_list_type *list = linked_list_create();
	
	int element[] = {2, 3, 5, 7, 11};
	int element_num = 5;
	int i;
	for (i = 0; i < element_num; i ++)
	{
		linked_list_insert_back(list, &element[i]);
	}

	//
	// insert "1" before "2"
	//
	linked_list_node_type *head_node = linked_list_search(list, &element[0], int_comparator, NULL);
	
	int new_head_node_data = 1;
	linked_list_node_type *new_head_node = linked_list_node_create();
	new_head_node->data = &new_head_node_data;
	
	linked_list_insert_before(list, head_node, new_head_node);

	//
	// insert "6" before "7"'
	//
	linked_list_node_type *node_7 = linked_list_search(list, &element[3], int_comparator, NULL);

	int new_node_6_value = 6;
	linked_list_node_type *new_node_6 = linked_list_node_create();
	new_node_6->data = &new_node_6_value;

	linked_list_insert_before(list, node_7, new_node_6);

	//
	// check if the element squence is correct
	//
	int new_element_squence[] = {1, 2, 3, 5, 6, 7, 11};
	int new_element_squence_length = 7;

	linked_list_node_type *node = list->head;
	for (i = 0; i < new_element_squence_length; i++)
	{
		if (!EXPECT_EQUAL(*TYPE_CAST(node->data, int *), new_element_squence[i]))
		{
			return FALSE;
		}

		node = node->next;	
	}

	linked_list_destroy(list, NULL);

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
		linked_list_node_type *node = linked_list_search(list, &element[i], int_comparator, NULL);
		int node_data = *TYPE_CAST(node->data, int *);
		if(!EXPECT_EQUAL(node_data, element[i]))
		{
			return FALSE;
		}

	}

	linked_list_destroy(list, NULL);

	return TRUE;
}

static bool
int_destroyer(void *int_data, va_list arg_list)
{
	free(int_data);
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
	linked_list_node_type *target = linked_list_search(list, &element[2], int_comparator, NULL);

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

	linked_list_destroy(list, destroy_int, NULL);

	return TRUE;
}

bool
linked_list_multi_dim_test()
{
	/*
	 * outer_list = [inner_list_1, inner_list_2, inner_list_3]
	 */
	linked_list_type *outer_list = linked_list_create();
	linked_list_type *inner_list_1 = linked_list_create();
	linked_list_type *inner_list_2 = linked_list_create();
	linked_list_type *inner_list_3 = linked_list_create();

	linked_list_insert_back(outer_list, inner_list_1);
	linked_list_insert_back(outer_list, inner_list_2);
	linked_list_insert_back(outer_list, inner_list_3);

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
		linked_list_insert_back(inner_list_1, p_data[i]);
	}

	for (i = 3; i < 6; i++)
	{
		linked_list_insert_back(inner_list_2, p_data[i]);
	}

	for (i = 6; i < 9; i++)
	{
		linked_list_insert_back(inner_list_3, p_data[i]);
	}

	linked_list_destroy(outer_list, linked_list_deconstructor, int_destroyer, NULL);

	return TRUE;
}

bool
linked_list_copy_test()
{
	//
	// Original linked list.
	//
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

	for (i = 0; i < element_num; i ++)
	{
		linked_list_insert_back(list, int_p[i]);
	}

	//
	// Copy the original list.
	//
	linked_list_type *list_copy = linked_list_copy(list, int_copier, NULL);

	//
	// Compare origin & copy, to make sure they are equal.
	//
	linked_list_node_type *origin_node = NULL;
	linked_list_node_type *copy_node = NULL;
	
	for (origin_node = list->head, copy_node = list_copy->head; origin_node != NULL && copy_node != NULL; origin_node = origin_node->next, copy_node = copy_node->next)
	{
		int origin_node_data = *TYPE_CAST(origin_node->data, int *);
		int copy_node_data = *TYPE_CAST(copy_node->data, int *);

		if (!EXPECT_EQUAL(origin_node_data, copy_node_data))
		{
			return FALSE;
		}
	}

	linked_list_destroy(list, destroy_int, NULL);
	linked_list_destroy(list_copy, destroy_int, NULL);

	return TRUE;
}



