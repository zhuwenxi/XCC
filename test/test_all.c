#include <stdio.h>
#include "test_all.h"




testcase testcases[TESTCASE_LIMIT] =
{
#define TESTCASE(description, function, desc_text) {.desc=description, .func=function, .text=desc_text},
#include "test.def"
#undef _TESTCASE
};

int main(int argc, char *args[])
{	
	int test_case_num = TESTCASE_LIMIT;
	int fail_case_num = 0;
	int pass_case_num = 0;
	
	int i;
	for (i = 0; i < test_case_num; i ++)
	{
		char *desc_text = testcases[i].text;
		printf("[%s]", desc_text);
		bool result = testcases[i].func();
		result ? pass_case_num ++ : fail_case_num ++;
		char *result_text = result ? " PASS.\n" : " FAIL!\n";
		printf("%s", result_text);
	}


	printf("All cases:  %d\n", pass_case_num);
	printf("Pass cases: %d\n", pass_case_num);
	printf("Fail cases: %d\n", fail_case_num);
}
