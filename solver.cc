#include "solver.h"
#include "d_math.h"
#include <assert.h>
#include <stddef.h>

void InitEquationCoeffs(struct EquationCoeffs* c){
	assert(c != NULL);

	c->coeff_of_sq_x = 0.;
	c->coeff_of_x = 0.;
	c->free_coeff = 0.;
}

void InitEquationSolutions(struct EquationSolutions* c){
	assert(c != NULL);

	c->root1 = 0.;
	c->root2 = 0.;
	c->nRoots = NO_ROOTS;
}

void SolveLinear(const struct EquationCoeffs* coeffs, struct EquationSolutions* roots){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");
	assert(roots != NULL);

	if (IsZero(coeffs->coeff_of_x)){
		roots->nRoots = (IsZero(coeffs->free_coeff)) ? INF_ROOTS : NO_ROOTS;
	}
	else{
		roots->root1 = -coeffs->free_coeff/coeffs->coeff_of_x;
		roots->nRoots = ONE_ROOT;
	}
}

void SolveQuadratic(const struct EquationCoeffs* coeffs, struct EquationSolutions* roots){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");
	assert(roots != NULL);

	double d = coeffs->coeff_of_x*coeffs->coeff_of_x - 4*coeffs->coeff_of_sq_x*coeffs->free_coeff;
	if (IsZero(d)){
		roots->root1 = -coeffs->coeff_of_x / (2 * coeffs->coeff_of_sq_x);
		roots->nRoots =  ONE_ROOT;
	}
	else if (d < 0){
		roots->nRoots =  NO_ROOTS;
	}
	else{
		double sqrt_d = sqrt(d);
		roots->root1 = (-coeffs->coeff_of_x + sqrt_d) / (2 * coeffs->coeff_of_sq_x);
		roots->root2 = (-coeffs->coeff_of_x - sqrt_d) / (2 * coeffs->coeff_of_sq_x);
		roots->nRoots = TWO_ROOTS;
	}
}

void SolveEquation(const struct EquationCoeffs* coeffs, struct EquationSolutions* roots){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");
	assert(roots != NULL);

	assert(CheckDouble(coeffs->coeff_of_sq_x));
	assert(CheckDouble(coeffs->coeff_of_x));
	assert(CheckDouble(coeffs->free_coeff));
	assert(roots != NULL);

	if (!IsZero(coeffs->coeff_of_sq_x)){
		SolveQuadratic(coeffs, roots);
	}else{
		SolveLinear(coeffs, roots);
	}
}

bool CompareSolutions(const struct EquationSolutions* a, const struct EquationSolutions* b){
	assert(a != NULL);
	assert(b != NULL);

	if (a->nRoots != b->nRoots) return false;
	if (a->nRoots == 1) return Equals(a->root1, b->root1) || Equals(a->root1, b->root2);
	if (a->nRoots == 2) return (Equals(a->root1, b->root1) && Equals(a->root2, b->root2)) || 
							  (Equals(a->root1, b->root2) && Equals(a->root2, b->root1));
	return true;
}

bool CompareCoeffs(const struct EquationCoeffs* a, const struct EquationCoeffs* b){
	assert(a != NULL);
	assert(b != NULL);

	return (a->coeff_of_sq_x == b->coeff_of_sq_x) &&
		   (a->coeff_of_x == b->coeff_of_x)       &&
		   (a->free_coeff == b->free_coeff);
}

