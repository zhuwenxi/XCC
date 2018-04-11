#include "util/DFA_test.h"
#include "util/DFA.h"
#include "util/NFA.h"
#include "stddefs.h"

bool
DFA_create_destroy_test()
{
	/*NFA_type *nfa = NFA_from_str("a");
	NFA_to_DFA(nfa);
	NFA_destroy(nfa, NULL);

	NFA_type *nfa2 = NFA_from_str("a|b");
	NFA_to_DFA(nfa2);
	NFA_destroy(nfa2, NULL);

	NFA_type *nfa3 = NFA_from_str("a*");
	NFA_to_DFA(nfa3);
	NFA_destroy(nfa3, NULL);

	NFA_type *nfa4 = NFA_from_str("(a)");
	NFA_to_DFA(nfa4);
	NFA_destroy(nfa4, NULL);*/

	NFA_type *nfa5 = NFA_from_str("(a|b)*abb");
	NFA_to_DFA(nfa5);
	NFA_destroy(nfa5, NULL);

	return TRUE;
}