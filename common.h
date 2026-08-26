#ifndef __UTIL__
#define __UTIL__
#include "io.h"

//! USE CAREFULLY! sizeof
#define ARR_LEN(X) sizeof(X)/sizeof(X[0])

#define debug_printf(fmt, ...) ColoredPrintf(CYAN, BLACK, fmt, ##__VA_ARGS__)

#endif /*__UTIL__*/