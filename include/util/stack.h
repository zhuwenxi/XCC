#ifndef __STACK_H__
#define __STACK_H__

#include "util/array_list.h"
#include "stddefs.h"

// stack is implemented by array list
typedef array_list_type stack_type;

stack_type *stack_create();

bool stack_destroy(stack_type *stack, void (*data_deconstructor)(void *));

bool stack_empty(stack_type *stack);

void stack_push(stack_type *stack, void *data);

void *stack_pop(stack_type *stack);

void *stack_peek(stack_type *stack);

#endif