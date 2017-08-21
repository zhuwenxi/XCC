#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

typedef void void_type;

#define TYPE_CAST(data, type) ((type)(data))

#define GET_DATA(node, type) (*((type *)(node->data)))

#define DECLARE_COMPARE(t) \
static bool \
t##_compare( t##_type *instance1, t##_type *instance2, ...) \
{ \
	if (instance1 == NULL || instance2 == NULL) \
	{\
		if (instance1 == NULL && instance2 == NULL) \
		{ \
			return TRUE; \
		} \
		else \
		{ \
			return FALSE;\
		} \
	}\
\
	va_list ap;\
	va_start(ap, instance2);\
	bool ret = t##_comparator(instance1, instance2, ap);\
	va_end(ap);\
\
	return ret;\
}

#define DECLARE_SEARCH(instance_type, node_type, instance_type_name) \
static node_type *\
instance_type_name##_search( instance_type *instance, void *data, bool (*comparator)(void *, void *, va_list), ...) \
{ \
	if (instance == NULL) \
	{\
		return NULL;\
	}\
\
	va_list ap;\
	va_start(ap, comparator);\
	node_type *ret = instance_type_name##_searcher(instance, data, comparator, ap);\
	va_end(ap);\
\
	return ret;\
}

#define DECLARE_DELETE(instance_type, instance_type_name) \
static bool \
instance_type_name##_delete( instance_type *instance, void *data, bool (*comparator)(void *, void *, va_list), ...) \
{ \
	if (instance == NULL) \
	{\
		return FALSE;\
	}\
\
	va_list ap;\
	va_start(ap, comparator);\
	bool ret = instance_type_name##_deletor(instance, data, comparator, ap);\
	va_end(ap);\
\
	return ret;\
}

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

#define DECLARE_DESTROY(t) \
static bool \
t##_destroy( t##_type *instance, ...) \
{ \
	if (instance == NULL) \
	{\
		return FALSE;\
	}\
\
	va_list ap;\
	va_start(ap, instance);\
	bool ret = t##_deconstructor(instance, ap);\
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

static inline bool
int_equal(void *a, void *b, va_list arg_list)
{
	if (a == b) return TRUE;
	if (a == NULL && b != NULL) return FALSE;
	if (a != NULL && b == NULL) return FALSE;

	int a_value = *TYPE_CAST(a, int *);
	int b_value = *TYPE_CAST(b, int *);

	return a_value == b_value;
}

static inline bool
int_comparator(void *a, void *b, va_list arg_list)
{
	if (a == NULL || b == NULL)
	{
		if (a == NULL && b == NULL)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	int a_val = *TYPE_CAST(a, int *);
	int b_val = *TYPE_CAST(b, int *);

	return a_val == b_val;
}

#endif
