#ifndef __REGEXP_H__
#define __REGEXP_H__

#include <stdarg.h>

#include "LR_automata.h"

typedef struct {
	char *text;
	LR_automata_type *DFA;
} regexp_type;

regexp_type *regexp_create(char *text);

bool regexp_deconstructor(regexp_type *regexp, va_list arg_list);
DECLARE_DESTROY(regexp);

char *regexp_search(char *pattern, char *str);

#endif