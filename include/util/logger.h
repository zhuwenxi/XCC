#ifndef __LOGGER_H__
#define __LOGGER_H__




#include <stdio.h>
#include "stddefs.h"

void LOG_impl(bool condition, char *text, ...);

#define LOG(condition, ...) \
	if (condition) {\
		printf("[%s:%d] ", __FILE__, __LINE__); \
		LOG_impl(condition, __VA_ARGS__); \
	}

#if defined(DEBUG)
#define DB_LOG(condition, ...) \
	LOG(condition, __VA_ARGS__)
#else
#define DB_LOG
#endif

#endif
