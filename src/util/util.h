#ifndef __UTIL_H__
#define __UTIL_H__

#include "regexp/regexp.h"

//
// Convert grammar token to string.
//
#define TO_STRING(token) #token

//
// DBCODE macro: for debug purpose.
//
#ifdef DEBUG
#define DBCODE(flag, code)\
do{\
\
	if(flag) {\
		code;\
	}\
} while(0)
#else
#define DBCODE(flag, code)
#endif

//
// Boolean type: true & false.
//
#define true 1
#define false 0
#endif
