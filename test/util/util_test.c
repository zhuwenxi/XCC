#include "util.h"
#include "logger.h"
#include "test.h"

bool
itoa_test()
{
	int i = 123;
	char *res = my_itoa(i);
	if (!EXPECT_STRING_EQUAL(res, "123"))
	{
		free(res);
		return FALSE;
	}
	else
	{
		free(res);
		return TRUE;
	}
	
}