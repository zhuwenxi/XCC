#ifndef __TEST_ALL_H__
#define __TEST_ALL_H__

#include "stddefs.h"
#include "util/linked_list_test.h"
#include "util/logger_test.h"

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
