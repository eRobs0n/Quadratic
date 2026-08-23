#ifndef __DMATH__
#define __DMATH__

#include <math.h>

const double EPS = 1e-9;

//! Checks if double isn't nan or inf
bool CheckDouble(double d);

/**
 * @note In case of a or b is nan or inf, returns false
 */
bool Equals(double a, double b);

bool IsZero(double d);

#endif /*__DMATH__*/