#ifndef __TEST_ALL_H__
#define __TEST_ALL_H__

#include "stddefs.h"
#include "linked_list_test.h"
#include "logger_test.h"

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

// extern testcase testcases[TESTCASE_LIMIT];

#endif
