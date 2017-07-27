#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <stdarg.h>

#define TYPE_CAST(data, type) ((type)(data))

#define GET_DATA(node, type) (*((type *)(node->data)))

#define DECLARE_COPY(t) \
static t##_type *\
t##_copy( t##_type *instance, ...) \
{ \
	if (instance == NULL) \
	{\
		return NULL;\
	}\
\
	va_list ap;\
	va_start(ap, instance);\
	t##_type *ret = t##_copier(instance, ap);\
	va_end(ap);\
\
	return ret;\
}


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

static inline void *
int_copier(void *origin_int, va_list arg_list)
{
	if (origin_int == NULL) return NULL;
	
	int *int_copy = create_int(*TYPE_CAST(origin_int, int *));

	return int_copy;
}

#endif
