#ifndef __TEST_H__
#define __TEST_H__




#include <stdio.h>
#include <string.h>
#include "stddefs.h"

#define EXPECT_TO_BE_EQUAL(a, b) \
	if (a != b) printf("[%s:%d] Oops! Expect %d and %d to be equal.\n", __FILE__, __LINE__, a, b);

#define EXPECT_STRING_TO_BE_EQUAL(a, b) \
	if (strcmp(a, b) != 0) printf("[%s:%d] Oops! Expect \"%s\" and \"%s\" to be equal.\n", __FILE__, __LINE__, a, b);
#endif
