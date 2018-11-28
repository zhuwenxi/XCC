#include "regexp.h"

bool regexp_create_destroy_test()
{
	regexp_type *regexp = regexp_create("(a|b)*abb");
	regexp_destroy(regexp, NULL);

	return TRUE;
}