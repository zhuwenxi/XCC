#include <stdio.h>
#include "test.h"

int main(int argc, char *argv[])
{
	printf("Hello, world!\n");
	EXPECT_NOT_EQUAL(1, 2);
	EXPECT_NOT_EQUAL(3, 3);
	EXPECT_TRUE( 2 > 1);
	EXPECT_TRUE( 3 > 4);
}
