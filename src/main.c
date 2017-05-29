#include "util/logger.h"
#include "util/string_buffer.h"

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");

	string_buffer str = string_buffer_create();
	string_buffer_append(&str, "Foo");
	LOG(TRUE, str);
	string_buffer_append(&str, "Bar");
	LOG(TRUE, str);
	string_buffer_append(&str, ": string_buffer");
	LOG(TRUE, str);

	string_buffer_destroy(str);
	
}
