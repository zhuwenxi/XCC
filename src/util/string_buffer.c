#include "util/string_buffer.h"
#include "util/logger.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

string_buffer
string_buffer_create()
{
	// allocate a buffer as empty string, which contains a '\0'
	string_buffer buffer = (string_buffer)malloc(sizeof(char) * STRING_BUFFER_ORIGIN_SIZE);
	buffer[0] = '\0';

	return buffer;
}

void
string_buffer_destroy(string_buffer buff)
{
	assert(buff != NULL);
	free(buff);
}


void
string_buffer_append(string_buffer *buff, char *text)
{
	size_t buffer_str_size = strlen(*buff);
	size_t text_str_size = strlen(text);

	*buff = (string_buffer)realloc(*buff, sizeof(char) * (buffer_str_size + text_str_size + 1));
	
	strcat(*buff, text);
}
