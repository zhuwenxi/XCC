#include "util/logger.h"
#include "util/string_buffer.h"
#include "util/linked_list.h"
#include "util/stack.h"
#include "util/hash_table.h"
#include "scanner/scanner.h"

#include <stdlib.h>
#include <stdarg.h>

#ifdef MTRACE
#include <mcheck.h>
#endif

static char *
str_to_str(void *data, va_list arg_list)
{
	string_buffer ret_str = string_buffer_create();
	string_buffer_append(&ret_str, "\"");

	char **str_ptr = (char **)data;
	string_buffer_append(&ret_str, *str_ptr);

	string_buffer_append(&ret_str, "\"");

	return ret_str;
}

static char *
int_to_str(void *data, va_list arg_list)
{
	string_buffer ret_str = string_buffer_create();
	string_buffer_append(&ret_str, "12306");

	return ret_str;
}

int main(int argc, char *argv[])
{
#ifdef MTRACE
	mtrace();
#endif

	printf("Hello, world!\n");

	scanner_main();

#ifdef MTRACE
	muntrace();
#endif
}
