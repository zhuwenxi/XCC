#ifndef __TEST_ALL_H__
#define __TEST_ALL_H__

#include "stddefs.h"
#define XTESTCASE(desc, func, text)
#define TESTCASE(desc, func, text) bool func();\

#include "test.def"
#undef XTESTCASE
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
#define XTESTCASE(desc, func, text)
#include "test.def"
TESTCASE_LIMIT
#undef TESTCASE
#undef XTESTCASE
} testcase_enum_type;

#endif
