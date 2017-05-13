#ifndef __TEST_H__
#define __TEST_H__




#include "stddefs.h"

typedef struct 
{
	int desc;
	bool (*func)();
	char *text;
} testcase;

typedef enum
{
#define DEFINE_TESTCASE(desc, func, text) desc,
#include "test.def"
TESTCASE_LIMIT
#undef DEFINE_TESTCASE
} testcase_enum_type;


extern testcase testcases[TESTCASE_LIMIT] =
{
#define DEFINE_TESTCASE(desc, func, text) {.desc=desc, .func=func, .text=text},
#include "test.def"
#undef DEFINE_TESTCASE
};

#endif
