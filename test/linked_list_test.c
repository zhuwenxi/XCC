#include <stdio.h>
#include "linked_list.h"
#include "linked_list_test.h"
#include "stddefs.h"




bool linked_list_test()
{
	if (linked_list_create_test() == TRUE)	
	{
		printf("linked_list_create() works properly\n");
	}
	else 
	{
		printf("Oops, linked_list_create() fails.\n");
	}
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



