#include "d_math.h"

bool CheckDouble(double d){
	return !isnan(d) && isfinite(d);
}

bool Equals(double a, double b){
	if (CheckDouble(a) && CheckDouble(b)) return fabs(a-b) < EPS;
	return false;
}

bool IsZero(double d){
	return Equals(d, 0.);
}