#ifndef __REGEXP_H__
#define __REGEXP_H__

#include <stdarg.h>

#include "LR_automata.h"

typedef enum
{
#define PRODUCTION_TOKEN(code, name) code,
#include "LR_automata_symbol.def"
#include "regexp_grammar.def"
#include "expression_grammar.def"
#undef PRODUCTION_TOKEN
	TOKEN_LIMIT
} regexp_token_type;

extern char *token_desc_table[];

typedef struct {
	char *text;
	LR_automata_type *DFA;
} regexp_type;

regexp_type *regexp_create(char *text);

bool regexp_deconstructor(regexp_type *regexp, va_list arg_list);
DECLARE_DESTROY(regexp);

char *regexp_search(char *pattern, char *str);

production_token_type regexp_grammar_get_token_type(char c);

void construct_ast_for_regexp_grammar(stack_type *ast_node_stack, production_type *prod_to_reduce);

#endif