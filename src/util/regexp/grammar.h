#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

/*
 * Context-free grammar
 */ 
typedef struct
{
	char *name;
	production_t productions[];
	unsigned int prod_nmbr;
} context_free_grammar;

#endif
