#ifndef __TEST_H__
#define __TEST_H__




#include <stdio.h>
#include <string.h>
#include "stddefs.h"

// a == b
#define EXPECT_EQUAL(a, b) \
	((a) != (b) ? printf("[%s:%d] Oops! Expect %d and %d to be equal.\n", __FILE__, __LINE__, a, b), FALSE : TRUE)

// a != b
#define EXPECT_NOT_EQUAL(a, b) \
	((a) == (b) ? printf("[%s:%d] Oops! Expect %d and %d to be not equal.\n", __FILE__, __LINE__, a, b), FALSE : TRUE)

// strcmp(a, b) == 0
#define EXPECT_STRING_EQUAL(a, b) \
	(strcmp(a, b) != 0 printf("[%s:%d] Oops! Expect \"%s\" and \"%s\" to be equal.\n", __FILE__, __LINE__, a, b), FALSE : TRUE)

// condition == TRUE
#define EXPECT_TRUE(condition) \
	(!(condition) ? printf("[%s:%d] Oops! Expect %s to be TRUE.\n", __FILE__, __LINE__, condition ? "TRUE" : "FALSE"), FALSE : TRUE)


#endif
