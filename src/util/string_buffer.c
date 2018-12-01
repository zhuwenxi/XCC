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

void
string_buffer_revert(string_buffer buff)
{
	int len = strlen(buff);

	int i;
	for (i = 0; i < len && i < len - 1 - i; i ++)
	{
		char tmp = buff[i];
		buff[i] = buff[len - 1 - i];
		buff[len - 1 - i] = tmp;
	}
}

int
string_buffer_search_char(string_buffer str, char c)
{
	int length = strlen(str);
	int index;
	for (index = 0; index < length; ++index)
	{
		if (str[index] == c)
		{
			return index;
		}
	}

	return -1;
}

string_buffer
string_buffer_substr(string_buffer str, int start, int end)
{
	string_buffer substr = string_buffer_create();
	int index;
	for (index = start; index < end; ++index)
	{
		char tmp[] = {str[index], '\0'};
		string_buffer_append(&substr, tmp);
	}

	return substr;
}

