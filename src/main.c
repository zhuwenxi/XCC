#include <stdio.h>
#include "test.h"

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");
	EXPECT_TO_BE_EQUAL(1, 2);
	EXPECT_STRING_TO_BE_EQUAL("fuck", "bitch");
	EXPECT_STRING_TO_BE_EQUAL("foo", "foo");
}
