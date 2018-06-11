#ifndef __DFA_H__
#define __DFA_H__

#include "util/NFA.h"
#include "util/linked_list.h"

typedef struct {
	int id;
	linked_list_type *nfa_states;
} DFA_state_type;

typedef struct {
	// start & end state:
	DFA_state_type *start;
	array_list_type *end;

	// all states:
	array_list_type *states;

	// transfer diagram:
	hash_table_type *transfer_diagram;
} DFA_type;

typedef struct {
	DFA_state_type *state;
	char *symbol;
} DFA_state_symbol_pair_type;

DFA_type *NFA_to_DFA(NFA_type *nfa);

DFA_type *DFA_create();

// de-constructor
bool DFA_deconstructor(DFA_type *self, va_list arg_list);
DECLARE_DESTROY(DFA)

DFA_state_type *DFA_state_create();

bool DFA_state_deconstructor(DFA_state_type *state, va_list arg_list);
DECLARE_DESTROY(DFA_state);

DFA_state_symbol_pair_type *DFA_state_symbol_pair_create(DFA_state_type *state, char *symbol);
bool DFA_state_symbol_pair_deconstructor(DFA_state_symbol_pair_type *self, va_list arg_list);
DECLARE_DESTROY(DFA_state_symbol_pair)


/*
* hash function for DFA_state:
*/
int DFA_state_symbol_pair_hash(void *key);

bool DFA_state_symbol_pair_compartor(void *one, void *another, va_list arg_list);

bool DFA_state_compartor(void *one, void *another, va_list arg_list);

char *DFA_state_debug_str(DFA_state_type *state, va_list arg_list);

char *get_DFA_debug_str(NFA_type *self);
#endif