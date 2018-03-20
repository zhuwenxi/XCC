#include "util/NFA_test.h"
#include "util/NFA.h"
#include "stddefs.h"

bool
NFA_create_destroy_test()
{
	NFA_type *nfa = NFA_from_str("a");
	NFA_destroy(nfa, NULL);

	NFA_type *nfa2 = NFA_from_str("a|b");
	NFA_destroy(nfa2, NULL);

	NFA_type *nfa3 = NFA_from_str("a*");
	NFA_destroy(nfa3, NULL);

	NFA_type *nfa4 = NFA_from_str("(a)");
	NFA_destroy(nfa4, NULL);

	NFA_type *nfa5 = NFA_from_str("(a|b)*abb");
	NFA_destroy(nfa5, NULL);

	return TRUE;
}