#include "util/logger.h"
#include "stddefs.h"




bool logger_test()
{
	LOG(TRUE, "logger_test()");
	LOG(TRUE, "Your name is %s", "foo");
	DB_LOG(TRUE, "DB_LOG");
	DB_LOG(FALSE, "DB_LOG: %s", "hi");
	return TRUE;
}
