#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "logger.h"




void LOG(bool condition, char *format, ...)
{
	if (condition)
	{
		size_t text_len = strlen(format) + 2;

		// create an empty string
		char new_text[text_len];
		new_text[0] = '\0';	
		
		// add a '\n' at the end of the string
		strcat(new_text, format);
		new_text[text_len - 2] = '\n';
		new_text[text_len - 1] = '\0';
		
		va_list ap;
		va_start(ap, format);
		printf("[%s:%d] ", __FILE__, __LINE__);
		vprintf(new_text, ap);
		va_end(ap);
	}
}