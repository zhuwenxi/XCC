#include "regexp.h"

bool regexp_create_destroy_test()
{
	regexp_type *regexp = regexp_create("(a|b)*abb");
	regexp_destroy(regexp, NULL);

	return TRUE;
}

bool regexp_search_test()
{
	regexp_return_group_type ret = regexp_search("(a|b)*abb", "abb");
	LOG(TRUE, "ret: %s", ret.str);
	if (ret.start != 0) return FALSE;
	if (ret.length != 3) return FALSE;
	if (strcmp(ret.str, "abb")) return FALSE;
	free(ret.str);

	regexp_return_group_type ret2 = regexp_search("(a|b)*abb", "fabbc");
	LOG(TRUE, "ret2: %s", ret2.str);
	if (ret2.start != 1) return FALSE;
	if (ret2.length != 3) return FALSE;
	if (strcmp(ret2.str, "abb")) return FALSE;
	free(ret2.str);

	regexp_return_group_type ret3 = regexp_search("fee|fie", "fif");
	if (ret3.start != -1) return FALSE;
	if (ret3.length != 0) return FALSE;
	if (ret3.str != NULL) return FALSE;
	free(ret3.str);

	regexp_return_group_type ret4 = regexp_search("fee|fie", "feex");
	LOG(TRUE, "ret4: %s", ret4.str);
	if (ret4.start != 0) return FALSE;
	if (ret4.length != 3) return FALSE;
	if (strcmp(ret4.str, "fee")) return FALSE;
	free(ret4.str);
	
	return TRUE;
}