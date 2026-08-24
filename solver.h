#ifndef __SOLVER__
#define __SOLVER__

#include "d_math.h"

enum EquationSolutionsNumber{
	INF_ROOTS =  -1, ///< Infinity number of roots
	NO_ROOTS =    0, ///< No roots
	ONE_ROOT =    1, ///< One root
	TWO_ROOTS =   2 ///< Two roots
};

struct EquationCoeffs{
	double coeff_of_sq_x;
	double coeff_of_x;
	double free_coeff;
};

struct EquationSolutions{
	double 					root1;
	double 					root2;
	EquationSolutionsNumber nRoots;
};

//! Initialize EquationCoeffs struct with 0
void InitEquationCoeffs(struct EquationCoeffs* c);

//! Initialize EquationSolutions struct with 0
void InitEquationSolutions(struct EquationSolutions* c);

//! Compares EquationSolutions structs
bool CompareSolutions(const struct EquationSolutions* a, const struct EquationSolutions* b);

//! Compares EquationCoeffs structs
bool CompareCoeffs(const struct EquationCoeffs* a, const struct EquationCoeffs* b);

/**
 * @param [in] coeffs Pointer to equation coefficients
 * @param [out] roots Pointer to the roots
*/
void SolveLinear(const struct EquationCoeffs* coeffs, struct EquationSolutions* roots);

/**
 * @param [in] coeffs Pointer to equation coefficients
 * @param [out] roots Pointer to the roots
*/
void SolveQuadratic(const struct EquationCoeffs* coeffs, struct EquationSolutions* roots);

/**
 * @param [in] coeffs Pointer to equation coefficients
 * @param [out] roots Pointer to the roots
*/
void SolveEquation(const struct EquationCoeffs* coeffs, struct EquationSolutions* roots);

#endif /*__SOLVER__*/

