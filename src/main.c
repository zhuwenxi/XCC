#include "util/logger.h"
#include "util/string_buffer.h"
#include "util/linked_list.h"
#include <stdlib.h>

static char *
str_to_str(void *data)
{

}

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");

	
	linked_list_type *list = linked_list_create();
	linked_list_insert_back(list, "a");
	linked_list_insert_back(list, "bb");
	linked_list_insert_back(list, "ccc");

	char *list_str = get_linked_list_debug_str(list, NULL);
	LOG(TRUE, list_str);

	free(list_str);

	linked_list_type *outer_list = linked_list_create();
	linked_list_type *inner_list_1 = linked_list_create();
	linked_list_type *inner_list_2 = linked_list_create();
	linked_list_type *inner_list_3 = linked_list_create();

	linked_list_insert_back(outer_list, inner_list_1);
	linked_list_insert_back(outer_list, inner_list_2);
	linked_list_insert_back(outer_list, inner_list_3);

	linked_list_insert_back(inner_list_1, "a");
	linked_list_insert_back(inner_list_1, "b");
	linked_list_insert_back(inner_list_1, "c");

	linked_list_insert_back(inner_list_2, "dd");
	linked_list_insert_back(inner_list_2, "ee");
	linked_list_insert_back(inner_list_2, "ff");

	linked_list_insert_back(inner_list_3, "ggg");
	linked_list_insert_back(inner_list_3, "hhh");
	linked_list_insert_back(inner_list_3, "iii");

	#define fuck linked_list_insert_back
	fuck(inner_list_3, "kkkk");

	list_str = get_linked_list_debug_str(outer_list, linked_list_debug_str, NULL);
	LOG(TRUE, list_str);
	free(list_str);
}
