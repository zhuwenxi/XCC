#ifndef __DFA_H__
#define __DFA_H__

#include "util/NFA.h"
#include "util/linked_list.h"

typedef struct {
	int id;
	linked_list_type *nfa_states;
} DFA_state_type;

typedef NFA_type DFA_type;

typedef NFA_state_symbol_pair_type DFA_state_symbol_pair_type;

DFA_type *NFA_to_DFA(NFA_type *nfa);

// borrow NFA's consturctor :)
#define DFA_create NFA_create

// de-constructor
bool DFA_deconstructor(DFA_type *self, va_list arg_list);
DECLARE_DESTROY(DFA)

DFA_state_type *DFA_state_create();

bool DFA_state_deconstructor(DFA_state_type *state);
DECLARE_DESTROY(DFA_state);


/*
* hash function for DFA_state:
*/
int DFA_state_symbol_pair_hash(void *key);

bool DFA_state_symbol_pair_compartor(void *one, void *another, va_list arg_list);



#endif