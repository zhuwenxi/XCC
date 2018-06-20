#include "util/DFA_test.h"
#include "util/DFA.h"
#include "util/NFA.h"
#include "stddefs.h"

bool
DFA_create_destroy_test()
{
	/*NFA_type *nfa = NFA_from_str("a");
	NFA_to_DFA(nfa);
	NFA_destroy(nfa, NULL);*/

	/*NFA_type *nfa2 = NFA_from_str("a|b");
	DFA_type *dfa2 = NFA_to_DFA(nfa2);
	DFA_destroy(dfa2, NULL);
	NFA_destroy(nfa2, NULL);*/

	/*NFA_type *nfa3 = NFA_from_str("a*");
	NFA_to_DFA(nfa3);
	NFA_destroy(nfa3, NULL);*/

	/*NFA_type *nfa4 = NFA_from_str("(a)");
	NFA_to_DFA(nfa4);
	NFA_destroy(nfa4, NULL);*/
		
	/*NFA_type *nfa5 = NFA_from_str("(a|b)*abb");
	DFA_type *dfa5 = NFA_to_DFA(nfa5);
	DFA_destroy(dfa5, NULL);
	NFA_destroy(nfa5, NULL);*/

	NFA_type *nfa6 = NFA_from_str("a(b|c)*");
	DFA_type *dfa6 = NFA_to_DFA(nfa6);
	DFA_destroy(dfa6, NULL);
	NFA_destroy(nfa6, NULL);

	return TRUE;
}