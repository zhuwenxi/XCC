#include "util.h"
#include "logger.h"

bool
itoa_test()
{
	int i = 10;
	LOG(TRUE, "itoa(): %s", itoa(i));
	return TRUE;
}