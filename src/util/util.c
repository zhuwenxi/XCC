#include "stddef.h"
#include "util.h"

char *
char_create(char c)
{
	char *c_ptr = (char *)malloc(sizeof(char));
	c_ptr[0] = c;
	return c_ptr;
}

bool
char_deconstructor(char *c_ptr)
{
	if (c_ptr != NULL)
		free(c_ptr);

	return TRUE;
}



