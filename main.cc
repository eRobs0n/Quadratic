#include <cctype>
#include <cmath>
#include <cstdio>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

const double EPS = 1e-9;

bool IsZero(double d){
	return fabs(d) < EPS;
}

bool CheckDouble(double d){
	return !isnan(d) && isfinite(d);
}

enum InputStatus{
	VALID_INPUT,
	INVALID_STR,
	INVALID_NUM
};

enum EquationRootsNumber{
	NO_ROOTS,
	ONE_ROOT,
	TWO_ROOTS,
	INF_ROOTS
};

/**
 * @param [in] coeff_of_x coefficient of x
 * @param [in] free_coeff free coefficient
 * @param [out] root Pointer to the root
 * @return Number of roots
 * @note In case of infinit number of roots returns SOLVER_INF_ROOTS
*/

int SolveLinear(double coeff_of_x, double free_coeff, double* root){
	assert(isfinite(coeff_of_x));
	assert(isfinite(free_coeff));
	assert(root != NULL);

	if (IsZero(coeff_of_x)){
		return (IsZero(free_coeff)) ? INF_ROOTS : 0;
	}
	else{
		*root = -free_coeff/coeff_of_x;
		return 1;
	}
}

/**
 * @param [in] coeff_of_square_x coefficient of x^2
 * @param [in] coeff_of_x coefficient of x
 * @param [in] free_coeff free coefficeint
 * @param [out] root1 Pointer to first root
 * @param [out] root2 Pointer to second root
 * @return Number of roots
 * @note In case of infinit number of roots returns INF_ROOTS
*/

int SolveQuadratic(double coeff_of_square_x, double coeff_of_x, double free_coeff, double* root1, double* root2){
	assert(isfinite(coeff_of_square_x));
	assert(isfinite(coeff_of_x));
	assert(root1 != NULL);
	assert(root2 != NULL);
	assert(root1 != root2);

	if (IsZero(coeff_of_square_x)){
		return SolveLinear(coeff_of_x, free_coeff, root1);
	}
	else{
		double d = coeff_of_x*coeff_of_x - 4*coeff_of_square_x*free_coeff;
		if (IsZero(d)){
			*root1 = -coeff_of_x / (2*coeff_of_square_x);
			return 1;
		}
		else if (d < 0){
			return 0;
		}
		else{
			double sqrt_d = sqrt(d);
			*root1 = (-coeff_of_x + sqrt_d) / (2 * coeff_of_square_x);
			*root2 = (-coeff_of_x - sqrt_d) / (2 * coeff_of_square_x);
			return 2;
		}
	}
}

int CheckStringOfDigits(char *s){
	assert(s != NULL);

	char c;
	while ((c = *s++) != '\0'){
		if (!isdigit(c))
			return 0;
	}
	return 1;
}

void ClearStdinBuffer(){
	int c = -1;
	while ((c = getchar()) != '\n' && c != EOF);
}

enum InputStatus EnterCoefficients(double* a, double* b, double* c){
	assert(a != NULL);
	assert(b != NULL);
	assert(c != NULL);

	char sep1 = 0, sep2 = 0, sep3 = 0;
	int argument_cnt = scanf("%lf%c%lf%c%lf%c", a, &sep1, b, &sep2, c, &sep3);
	if (argument_cnt != 6 || sep1 != ' ' || sep2 != ' ' || sep3 != '\n'){
		return INVALID_STR;
	}
	if (!CheckDouble(*a) || !CheckDouble(*b) || !CheckDouble(*c)){
		return INVALID_NUM;
	}
	return VALID_INPUT;
}

void PrintRoots(int nRoots, double x1, double x2, int digits_after_point){
	switch (nRoots){
		case NO_ROOTS:
			printf("No roots\n");
			break;
		case INF_ROOTS:
			printf("Infinity number of roots\n");
			break;
		case ONE_ROOT:
			printf("x = %.*lf\n", digits_after_point, x1);
			break;
		case TWO_ROOTS:
			printf("x1 = %.*lf, x2 = %.*lf\n", digits_after_point, x1, digits_after_point, x2);
			break;
		default:
			printf("Something went wrong...\n");
			break;
	}
	
}

int main(int argc, char* argv[]){
	double a = 0., b = 0., c = 0.;

	int digits_after_point = 2;

	if (argc>1){
		if (strcmp(argv[1], "-dap") == 0 && CheckStringOfDigits(argv[2])){
			digits_after_point = atoi(argv[2]);
		}else{
			printf("Usage: qdr -dap digits_after_point\n");
			return -1;
		}
	}

	printf("Quadratic Equation Solver\n ax\u00B2 + bx + c = 0\n"); //\u00B2 - unicode ² symbol

	printf("Enter coefficients: ");

	enum InputStatus input_result = EnterCoefficients(&a, &b, &c);
	while (input_result != VALID_INPUT){
		if (input_result == INVALID_NUM){
			printf("Invalid numbers. Don't use nan or inf\nEnter coefficients: ");
		}else if(input_result == INVALID_STR){
			printf("Invalid input. Please enter 3 numbers separated by space\nEnter coefficients: ");
			ClearStdinBuffer();
		}else{
			printf("Please try again.\n");
		}
		input_result = EnterCoefficients(&a, &b, &c);
	}

	printf("You entered %.*lfx\u00B2 %c %.*lfx %c %.*lf = 0 equation.\n", 
		                     digits_after_point,      a, 
		(b>=0) ? '+' : '-', digits_after_point, fabs(b), 
		(c>=0) ? '+' : '-', digits_after_point, fabs(c)
		); 


	double x1 = 0., x2 = 0.;
	int nRoots = SolveQuadratic(a, b, c, &x1, &x2);

	PrintRoots(nRoots, x1, x2, digits_after_point);

	return 0;
}
