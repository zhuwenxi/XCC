#ifndef __LOGGER_H__
#define __LOGGER_H__




#include "stddefs.h"

void LOG(bool condition, char *text, ...);


#if defined(DEBUG)
#define DB_LOG(condition, ...) \
	LOG(condition, __VA_ARGS__)
#else
#define DB_LOG
#endif

#endif
