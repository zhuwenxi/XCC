#ifndef __DFA_H__
#define __DFA_H__

#include "util/NFA.h"

typedef NFA_state_type DFA_state_type;

typedef NFA_type DFA_type;

typedef NFA_state_symbol_pair_type DFA_state_symbol_pair_type;

DFA_type *NFA_to_DFA(NFA_type *nfa);

#endif