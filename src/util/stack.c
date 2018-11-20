#include <stdlib.h>

#include "util/stack.h"
#include "util/util.h"
#include "util/logger.h"

// stack_type *
// stack_create()
// {
// 	stack_type *stack = array_list_create();
// 	return stack;
// }

// bool
// stack_destroy(stack_type *stack, void (*data_deconstructor)(void *))
// {
// 	return array_list_destroy(stack, data_deconstructor);
// }

bool
stack_empty(stack_type *stack)
{
	return stack->length == 0;
}

void
stack_push(stack_type *stack, void *data)
{
	array_list_append(stack, data);
}

void *
stack_pop(stack_type *stack)
{
	stack->length --;

	array_list_node_type *top_node = stack->content[stack->length];
	void *top_data = top_node->data;
	free(top_node);
	stack->content[stack->length] = NULL;

	return top_data;
}

void *
stack_peek(stack_type *stack)
{
	return array_list_get(stack, stack->length - 1);
}


