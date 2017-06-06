#include "util/logger.h"
#include "util/string_buffer.h"
#include "util/array_list.h"
#include <stdlib.h>

static char *
str_to_str(void *data)
{

}

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");

	
	array_list_type *list = array_list_create();
	array_list_append(list, "a");
	array_list_append(list, "bb");
	array_list_append(list, "ccc");

	char *list_str = get_array_list_debug_str(list, NULL);
	LOG(TRUE, list_str);

	free(list_str);

	array_list_type *outer_list = array_list_create();
	array_list_type *inner_list_1 = array_list_create();
	array_list_type *inner_list_2 = array_list_create();
	array_list_type *inner_list_3 = array_list_create();

	array_list_append(outer_list, inner_list_1);
	array_list_append(outer_list, inner_list_2);
	array_list_append(outer_list, inner_list_3);

	array_list_append(inner_list_1, "a");
	array_list_append(inner_list_1, "b");
	array_list_append(inner_list_1, "c");

	array_list_append(inner_list_2, "dd");
	array_list_append(inner_list_2, "ee");
	array_list_append(inner_list_2, "ff");

	array_list_append(inner_list_3, "ggg");
	array_list_append(inner_list_3, "hhh");
	array_list_append(inner_list_3, "iii");

	list_str = get_array_list_debug_str(outer_list, array_list_debug_str, NULL);
	LOG(TRUE, list_str);
	free(list_str);
}
