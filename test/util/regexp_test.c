#include "regexp.h"

bool regexp_create_destroy_test()
{
	regexp_type *regexp = regexp_create("(a|b)*abb");
	regexp_destroy(regexp, NULL);

	return TRUE;
}

bool regexp_search_test()
{
	char *ret = regexp_search("(a|b)*abb", "abb");
	LOG(TRUE, "ret: %s", ret);

	char *ret2 = regexp_search("(a|b)*abb", "fabbc");
	LOG(TRUE, "ret2: %s", ret);
	
	return TRUE;
}