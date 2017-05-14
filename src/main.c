#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "util/util.h"
#include "util/linked_list.h"
#include "util/logger.h"

static char*
str_to_str(void *data)
{
	char *origin_data = TYPE_CAST(data, char *);
	size_t size = strlen(origin_data) + 1;
	
	char *ret_str = (char *)malloc(sizeof(char) * size);

	strcpy(ret_str, origin_data);
	
	return ret_str;
}

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");

	linked_list_type *list = linked_list_create();
	
	linked_list_insert_back(list, "Abandon");
	linked_list_insert_back(list, "Betray");
	linked_list_insert_back(list, "Cheat");
	linked_list_insert_back(list, "Deconstruct");

	LOG(TRUE, linked_list_debug_str(list, str_to_str));
}
