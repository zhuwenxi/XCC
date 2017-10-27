#include "util.h"
#include "logger.h"
#include "test.h"

bool
itoa_test()
{
	int i = 123;
	if (!EXPECT_STRING_EQUAL(itoa(i), "123")) return FALSE;
	return TRUE;
}