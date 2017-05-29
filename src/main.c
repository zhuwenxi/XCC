#include "util/logger.h"
#include "util/string_buffer.h"
#include "util/array_list.h"

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");

	
	array_list_type *list = array_list_create();
	array_list_append(list, "a");
	array_list_append(list, "bb");
	array_list_append(list, "ccc");

	LOG(TRUE, array_list_debug_str(list, NULL));
}
