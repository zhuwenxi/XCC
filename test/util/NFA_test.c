#include "util/NFA_test.h"
#include "util/NFA.h"
#include "stddefs.h"

bool
NFA_create_destroy_test()
{
	NFA_type *nfa = NFA_from_str("ab");
	//NFA_type *nfa = NFA_from_str("(a|b)*abb");
	//NFA_destroy(nfa, NULL);

	return TRUE;
}