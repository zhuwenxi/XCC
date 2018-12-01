#include <stdio.h>
#include "test_all.h"

#ifdef MTRACE
#include <mcheck.h>
#endif

#ifdef WINDOWS
#include <crtdbg.h>
#endif


testcase testcases[TESTCASE_LIMIT] =
{
#define TESTCASE(description, function, desc_text) {.desc=description, .func=function, .text=desc_text},
#define XTESTCASE(desc, func, text)
#include "test.def"
#undef TESTCASE
#undef XTESTCASE
};

int main(int argc, char *args[])
{	
#ifdef WINDOWS
	//_CrtSetBreakAlloc(640174);
#endif

	int test_case_num = TESTCASE_LIMIT;
	int fail_case_num = 0;
	int pass_case_num = 0;

#ifdef MTRACE
	mtrace();
#endif

	int i;
	for (i = 0; i < test_case_num; i ++)
	{
		bool result = testcases[i].func();
		result ? pass_case_num ++ : fail_case_num ++;
		char *result_text = result ? "PASS. " : "FAIL! ";
		printf("%s", result_text);
		char *desc_text = testcases[i].text;
		printf("[%s]\n", desc_text);
	}

	printf("\n==============================================\n\n");
	printf("All cases:  %d\n", pass_case_num + fail_case_num);
	printf("Pass cases: %d\n", pass_case_num);
	printf("Fail cases: %d\n\n", fail_case_num);

#ifdef MTRACE
	muntrace();
#endif

#ifdef WINDOWS
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
