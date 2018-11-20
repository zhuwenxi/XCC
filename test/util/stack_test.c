#include "util/stack_test.h"
#include "util/stack.h"
#include "test.h"
#include "util/util.h"
#include "logger.h"

bool
stack_create_destroy_test()
{
	stack_type *stack = stack_create();
	stack_destroy(stack, NULL);

	return TRUE;
}

bool
stack_empty_test()
{
	stack_type *stack = stack_create();

	if (!EXPECT_EQUAL(stack_empty(stack), TRUE)) return FALSE;

	int data = 1;
	stack_push(stack, &data);

	if (!EXPECT_EQUAL(stack_empty(stack), FALSE)) return FALSE;

	stack_destroy(stack, NULL);
	return TRUE;
}

bool
stack_push_pop_test()
{
	stack_type *stack = stack_create();

	int len = 30;
	int *data = (int *)malloc(sizeof(int) * len);

	int i;
	for (i = 0; i < len; i++)
	{
		data[i] = i;
		stack_push(stack, &data[i]);
	}

	if (!EXPECT_EQUAL(stack_empty(stack), FALSE)) return FALSE;
	if (!EXPECT_EQUAL((int)stack->length, len)) return FALSE;

	for (i = len - 1; i >= 0; i --)
	{
		int d = *TYPE_CAST(stack_pop(stack), int *);
		if (!EXPECT_EQUAL(d, i)) return FALSE;
	}

	if (!EXPECT_EQUAL(stack_empty(stack), TRUE)) return FALSE;

	stack_destroy(stack, NULL);

	free(data);
	return TRUE;
}

bool
stack_peek_test()
{
	stack_type *stack = stack_create();

	int len = 30;
	int *data = (int *)malloc(sizeof(int) * len);

	int i;
	for (i = 0; i < len; i++)
	{
		stack_push(stack, &data[i]);
	}

	if (!EXPECT_EQUAL(stack_empty(stack), FALSE)) return FALSE;

	for (i = len - 1; i >= 0; i --)
	{ 		
		data[i] = i;
		int d = *TYPE_CAST(stack_peek(stack), int *);
		if (!EXPECT_EQUAL(d, len - 1)) return FALSE;
	}

	if (!EXPECT_EQUAL(stack_empty(stack), FALSE)) return FALSE;
	
	stack_destroy(stack, NULL);
	free(data);

	return TRUE;
}