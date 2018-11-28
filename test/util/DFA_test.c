#include "util/DFA_test.h"
#include "util/DFA.h"
#include "util/NFA.h"
#include "stddefs.h"

bool
DFA_create_destroy_test()
{
	NFA_type *nfa = NFA_from_str("a");
	DFA_type *dfa = NFA_to_DFA(nfa);
	DFA_destroy(dfa, NULL);

	NFA_type *nfa2 = NFA_from_str("a|b");
	DFA_type *dfa2 = NFA_to_DFA(nfa2);
	DFA_destroy(dfa2, NULL);

	NFA_type *nfa3 = NFA_from_str("a*");
	DFA_type *dfa3 = NFA_to_DFA(nfa3);
	DFA_destroy(dfa3, NULL);

	NFA_type *nfa4 = NFA_from_str("(a)");
	DFA_type *dfa4 = NFA_to_DFA(nfa4);
	DFA_destroy(dfa4, NULL);
		
	NFA_type *nfa5 = NFA_from_str("(a|b)*abb");
	DFA_type *dfa5 = NFA_to_DFA(nfa5);
	DFA_destroy(dfa5, NULL);

	NFA_type *nfa6 = NFA_from_str("a(b|c)*");
	DFA_type *dfa6 = NFA_to_DFA(nfa6);
	DFA_destroy(dfa6, NULL);

	return TRUE;
}