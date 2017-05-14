#ifndef __TEST_ALL_H__
#define __TEST_ALL_H__

#include "stddefs.h"
#define TESTCASE(desc, func, text) bool func();\

#include "test.def"
#undef TESTCASE

typedef struct 
{
	int desc;
	bool (*func)();
	char *text;
} testcase;

typedef enum
{
#define TESTCASE(desc, func, text) desc,
#include "test.def"
TESTCASE_LIMIT
#undef TESTCASE
} testcase_enum_type;

#endif
