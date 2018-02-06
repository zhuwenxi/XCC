#include "regexp.h"


regexp_type *
regexp_create(char *text)
{
	regexp_type *self = (regexp_type *)malloc(sizeof(regexp_type));
	self->text = text;

	return self;
}

bool
regexp_deconstructor(regexp_type *regexp, va_list arg_list)
{
	bool dfa_destroyed = LR_automata_destroy(regexp->DFA);
	free(regexp);
	
	return dfa_destroyed;
}

static void
thompson_consturction()
{

}

char *regexp_search(char *pattern, char *str)
{
	
}