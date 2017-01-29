#ifndef __TEST_H__
#define __TEST_H__

#include "../src/util/util.h"

#define TEST_SUITE(func, desc) bool func();
#include "test.def"
#undef TEST_SUITE

typedef struct
{
	bool (*func)();
	char *desc;
} test_suite_entry_t;

test_suite_entry_t test_suite_table[] =
{
#define TEST_SUITE(function, description) {.func=function, .desc=description},
#include "test.def"
#undef TEST_SUITE
{.func = NULL, .desc = NULL},
}; 

inline test_suite_print_all()
{
	int i;
	for (i = 0; test_suite_table[i].func != NULL; i++)
	{
		printf("%s\n", test_suite_table[i].desc);
	}
}

inline test_suite_run_all()
{
	int i;
	int test_suite_count = 0;
	for (i = 0; test_suite_table[i].func != NULL; i++)
	{
		bool pass = test_suite_table[i].func();
		if (!pass) {
			printf("\nCase fails: %s\n", test_suite_table[i].desc);	
			return;
		} else {
			printf("*");
		}
		test_suite_count ++;
	}

	printf("\nAll %d cases passed.\n", test_suite_count);
}

#endif
