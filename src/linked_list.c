#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>




linked_list_type *
linked_list_create()
{
	linked_list_type *list = (linked_list_type *)malloc(sizeof(linked_list_type));
	list->head = NULL;
	return list;
}
