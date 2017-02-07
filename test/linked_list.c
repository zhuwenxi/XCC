#include <stdio.h>
#include "../src/util/util.h"

bool linked_list_create_destroy_test()
{
	linked_list_t *list = linked_list_create();
	linked_list_destroy(list, NULL);
	return true;
}

bool linked_list_append_test()
{
	linked_list_t *list = linked_list_create();
	linked_list_append(list, (void *)1);
	// linked_list_append(list, (void *)2);

	linked_list_destroy(list, NULL);
	return true;
}

bool linked_list_iteration_test()
{
	linked_list_t *list = linked_list_create();

	int nmbr1 = 1;
	int nmbr2 = 2;
	int nmbr3 = 3;

	linked_list_append(list, (void *)&nmbr1);
	linked_list_append(list, (void *)&nmbr2);
	linked_list_append(list, (void *)&nmbr3);

	linked_list_node_t *node;
	for (node = list->head; node != NULL; node = node->next)
	{
		int *disp_nmbr = LL_NODE_DATA(node, int *);
	}

	linked_list_destroy(list, NULL);

	return true;
}
