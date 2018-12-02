#ifndef __REGEXP_H__
#define __REGEXP_H__

#include <stdarg.h>

#include "LR_automata.h"
#include "NFA.h"
#include "DFA.h"
#include "opts.h"

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
	DFA_type *DFA;
} regexp_type;

// "Group" return by regexp_search()
typedef struct {
	int start;
	int length;
	char *str;
} regexp_return_group_type;

regexp_type *regexp_create(char *text);

bool regexp_deconstructor(regexp_type *regexp, va_list arg_list);
DECLARE_DESTROY(regexp);

regexp_return_group_type regexp_search(char *pattern, char *str);

regexp_return_group_type regexp_match(char *pattern, char *str);

production_token_type regexp_grammar_get_token_type(char *str);

void construct_ast_for_regexp_grammar(stack_type *ast_node_stack, production_type *prod_to_reduce);

#endif