#ifndef __STACK_H__
#define __STACK_H__

#include <stdarg.h>

#include "util/array_list.h"
#include "stddefs.h"

// stack is implemented by array list
typedef array_list_type stack_type;

// stack_type *stack_create();
#define stack_create array_list_create

// bool stack_destroy(stack_type *stack, void (*data_deconstructor)(void *));
#define stack_destroy array_list_destroy

bool stack_empty(stack_type *stack);

void stack_push(stack_type *stack, void *data);

void *stack_pop(stack_type *stack);

void *stack_peek(stack_type *stack);

#define get_stack_debug_str get_array_list_debug_str
#define stack_debug_str array_list_debug_str

#endif