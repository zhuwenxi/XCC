#include <stdio.h>
#include "linked_list.h"
#include "linked_list_test.h"
#include "stddefs.h"




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



