#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <stdarg.h>

#define TYPE_CAST(data, type) ((type)(data))

#define GET_DATA(node, type) (*((type *)(node->data)))

static inline int *
create_int(int i)
{
	int *ptr_i = (int *)malloc(sizeof(int));
	*ptr_i = i;

	return ptr_i;
}

static inline void
int_deconstructor(int *p, va_list arg_list)
{
	free(p);
}

#endif
