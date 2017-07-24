#include "LR_automata.h"
#include "array_list.h"

#include <stdlib.h>

void
construct_canonical_collection(LR_automata_type *lr_automata)
{

}

LR_automata_type *
LR_automata_create(context_free_grammar_type *grammar)
{
	LR_automata_type *lr_automata = (LR_automata_type *)malloc(sizeof(LR_automata_type));
	lr_automata->LR_items = array_list_create();
	
	construct_canonical_collection(lr_automata);

	return lr_automata;
}

bool 
LR_automata_destory(LR_automata_type *lr_automata, ...)
{
	if (lr_automata == NULL)
	{
		return FALSE;
	}

	va_list ap;
	va_start(ap, lr_automata);
	LR_automata_deconstructor(lr_automata, ap);
	va_end(ap);

	return TRUE;
}

bool
LR_automata_deconstructor(LR_automata_type *lr_automata, va_list arg_list)
{
	if (lr_automata == NULL)
	{
		return FALSE;
	}

	array_list_deconstructor(lr_automata->LR_items, arg_list);

	free(lr_automata);

	return TRUE;
}