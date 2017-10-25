#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

typedef char * string_buffer;

string_buffer string_buffer_create();
void string_buffer_append(string_buffer *buff, char *text);
void string_buffer_destroy(string_buffer buff);
void string_buffer_revert(string_buffer buff);

#define STRING_BUFFER_ORIGIN_SIZE 1
#endif