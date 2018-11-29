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
	if (ret.start != 0) return FALSE;
	if (ret.length != 3) return FALSE;
	if (strcmp(ret.str, "abb")) return FALSE;
	free(ret.str);

	regexp_return_group_type ret2 = regexp_search("(a|b)*abb", "fabbc");
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
	if (ret4.start != 0) return FALSE;
	if (ret4.length != 3) return FALSE;
	if (strcmp(ret4.str, "fee")) return FALSE;
	free(ret4.str);

	regexp_return_group_type ret5 = regexp_search("(a|b)*abb", "aabbabbb");
	if (ret5.start != 0) return FALSE;
	if (ret5.length != 7) return FALSE;
	if (strcmp(ret5.str, "aabbabb")) return FALSE;
	free(ret5.str);

	regexp_return_group_type ret6 = regexp_search("0x1234", "0x123a");
	if (ret6.start != -1) return FALSE;
	if (ret6.length != 0) return FALSE;
	if (ret6.str != NULL) return FALSE;
	free(ret6.str);
	
	return TRUE;
}