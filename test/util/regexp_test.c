#include "regexp.h"
#include "test.h"

bool regexp_create_destroy_test()
{
	regexp_type *regexp = regexp_create("(a|b)*abb");
	regexp_destroy(regexp, NULL);

	return TRUE;
}

static bool
check_result(regexp_return_group_type ret, int start, int length, char *str)
{
	if (!EXPECT_EQUAL(ret.start, start)) return FALSE;
	if (!EXPECT_EQUAL(ret.length, length)) return FALSE;
	if (ret.str != str && !EXPECT_STRING_EQUAL(ret.str, str)) return FALSE;

	return TRUE;
}

bool regexp_search_test()
{
	regexp_return_group_type ret = regexp_search("(a|b)*abb", "abb");
	if (!check_result(ret, 0, 3, "abb")) return FALSE;
	free(ret.str);

	regexp_return_group_type ret2 = regexp_search("(a|b)*abb", "fabbc");
	if (!check_result(ret2, 1, 3, "abb")) return FALSE;
	free(ret2.str);

	regexp_return_group_type ret3 = regexp_search("fee|fie", "fif");
	if (!check_result(ret3, -1, 0, NULL)) return FALSE;
	free(ret3.str);

	regexp_return_group_type ret4 = regexp_search("fee|fie", "feex");
	if (!check_result(ret4, 0, 3, "fee")) return FALSE;
	free(ret4.str);

	regexp_return_group_type ret5 = regexp_search("(a|b)*abb", "aabbabbb");
	if (!check_result(ret5, 0, 7, "aabbabb")) return FALSE;
	free(ret5.str);

	regexp_return_group_type ret6 = regexp_search("0x1234", "0x123a");
	if (!check_result(ret6, -1, 0, NULL)) return FALSE;
	free(ret6.str);

	regexp_return_group_type ret7 = regexp_search("_*", "a_____b");
	if (!check_result(ret7, 1, 5, "_____")) return FALSE;
	free(ret7.str);
	
	regexp_return_group_type ret8 = regexp_search("a\\+b", "a+b");
	if (!check_result(ret8, 0, 3, "a+b")) return FALSE;
	free(ret8.str);

	regexp_return_group_type ret9 = regexp_search("a b", " a b");
	if (!check_result(ret9, 1, 3, "a b")) return FALSE;
	free(ret9.str);

	regexp_return_group_type ret10 = regexp_search("a\\*b", "a*b");
	if (!check_result(ret10, 0, 3, "a*b")) return FALSE;
	free(ret10.str);

	regexp_return_group_type ret11 = regexp_search("[0-9]*", "123");
	if (!check_result(ret11, 0, 3, "123")) return FALSE;
	free(ret11.str);

	regexp_return_group_type ret12 = regexp_search("[_a-zA-Z][_a-zA-Z0-9]*", "hello_world");
	if (!check_result(ret12, 0, 11, "hello_world")) return FALSE;
	free(ret12.str);

	return TRUE;
}