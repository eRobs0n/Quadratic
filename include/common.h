#ifndef __UTIL__
#define __UTIL__
#include "io.h"

#define MIN(A, B) A<=B ? A : B
#define MAX(A, B) A>=B ? A : B

//! USE CAREFULLY! sizeof
#define ARR_LEN(X) sizeof(X)/sizeof(X[0])

//! Print cyan text in black backround for debug
#define debug_printf(fmt, ...) ColoredPrintf(CYAN, BLACK, fmt, ##__VA_ARGS__)

//---/\---/\-------Это ASCII KOT!--//
//  {  '-'  }                      //
//  {  0 0  }     Добавь его себе  //
//  --> V <--  в исходник, и тебе  //
//   \ \|/ /      будет, наверно,  //
//    \___/  приятно отлаживаться  //
//---------------долгими ночами:)--//

#endif /*__UTIL__*/