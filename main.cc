#include <cstdio>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

#define SOLVER_INF_ROOTS -1
#define EPS 1e-9
#define MAX_INPUT_LENGTH 100

/**
 * @param [in] a a-coefficient
 * @param [in] b b-coefficient
 * @param [out] x Pointer to the root
 * @return Number of roots
 * @note In case of infinit number of roots returns SOLVER_INF_ROOTS
*/

int SolveLinear(double a, double b, double* x){
	assert(isfinite(a));
	assert(isfinite(b));
	assert(x != NULL);

	if (abs(a) < EPS)
		return (abs(b) < EPS) ? SOLVER_INF_ROOTS : 0;
	else{
		*x = -b/a;
		return 1;
	}
}

/**
 * @param [in] a a-coefficient
 * @param [in] b b-coefficient
 * @param [in] c c-coefficient
 * @param [out] x1 Pointer to first root
 * @param [out] x2 Pointer to second root
 * @return Number of roots
 * @note In case of infinit number of roots returns SOLVER_INF_ROOTS
*/

int SolveQuadratic(double a, double b, double c, double* x1, double* x2){
	assert(isfinite(a));
	assert(isfinite(b));
	assert(x1 != NULL);
	assert(x2 != NULL);
	assert(x1 != x2);

	if (abs(a) < EPS){
		return SolveLinear(b, c, x1);
	}
	else{
		double d = b*b - 4*a*c;
		if (fabs(d) < EPS){
			*x1 = -b / (2*a);
			return 1;
		}
		else if (d < 0){
			return 0;
		}
		else{
			*x1 = (-b + sqrt(d)) / (2 * a);
			*x2 = (-b - sqrt(d)) / (2 * a);
			return 2;
		}
	}
}

int CheckStringOfDigits(char *s){
	char c;
	while ((c = *s++) != '\0'){
		if (!isdigit(c))
			return 0;
	}
	return 1;
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
	printf("Enter coefficients a, b, c: ");

	char trash_symbols;
	char user_input[MAX_INPUT_LENGTH];
	fgets(user_input, MAX_INPUT_LENGTH, stdin);
	int input_param_count = sscanf(user_input, "%lf %lf %lf %c", &a, &b, &c, &trash_symbols);

	while (input_param_count != 3){
		printf("Incorrect input\nEnter coefficients a, b, c: ");
		a = 0., b = 0., c = 0.;
		fgets(user_input, MAX_INPUT_LENGTH, stdin);
		input_param_count = sscanf(user_input, "%lf %lf %lf %c", &a, &b, &c, &trash_symbols);
	}

	printf("You entered %.*lfx\u00B2 %c %.*lfx %c %.*lf = 0 equation.\n", digits_after_point,
	a, (b>0) ? '+' : '-', digits_after_point, fabs(b), (c>0) ? '+' : '-', digits_after_point, fabs(c)); 

	double x1 = 0., x2 = 0.;
	int nRoots = SolveQuadratic(a, b, c, &x1, &x2);

	if (nRoots == 0) 
		printf("No roots\n");
	else if (nRoots == SOLVER_INF_ROOTS)
		printf("Infinity number of roots\n");
	else if(nRoots == 1)
		printf("x = %.*lf\n", digits_after_point, x1);
	else if (nRoots == 2)
		printf("x1 = %.*lf, x2 = %.*lf\n", digits_after_point, x1, digits_after_point, x2);
	else
		printf("Something went wrong...\n");

	return 0;
}
