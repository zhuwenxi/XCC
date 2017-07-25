#include "util/logger.h"
#include "util/string_buffer.h"
#include "util/linked_list.h"
#include "util/stack.h"
#include "util/hash_table.h"
#include "scanner/scanner.h"

#include <stdlib.h>
#include <stdarg.h>

#ifdef MTRACE
#include <mcheck.h>
#endif

int main(int argc, char *argv[])
{
#ifdef MTRACE
	mtrace();
#endif

	printf("Hello, world!\n");

	scanner_main();

#ifdef MTRACE
	muntrace();
#endif
}
