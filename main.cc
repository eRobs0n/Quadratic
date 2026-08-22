#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

const double EPS = 1e-9;

const int COLOR_STR_MAX_LEN = 100;
const char* CONSOLE_RESET = "\033[0m\033[K";

const int MAX_ROOTS = 2;
const int MAX_COEFF_CNT = 3;

enum ConsoleColors{
	NO_COLOR = -1,
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE
};



void CreateColorStr(char* res_color, size_t res_color_len, enum ConsoleColors text_color, enum ConsoleColors bg_color){
	if (text_color != NO_COLOR && bg_color != NO_COLOR) {
		snprintf(res_color, res_color_len, "\033[3%d;4%dm", text_color, bg_color);
	}
	if (text_color == NO_COLOR){
		snprintf(res_color, res_color_len, "\033[4%dm", bg_color);
	}
	if (bg_color == NO_COLOR){
		snprintf(res_color, res_color_len, "\033[3%dm", text_color);
	}
}

void ColoredPrintf(enum ConsoleColors text_color, enum ConsoleColors bg_color, const char* fmt, ...){
	char res_color[COLOR_STR_MAX_LEN];
	CreateColorStr(res_color, COLOR_STR_MAX_LEN, text_color, bg_color);
	printf("%s", res_color);
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("%s", CONSOLE_RESET);
}

bool IsZero(double d){
	return fabs(d) < EPS;
}

bool Equals(double a, double b){
	return fabs(a-b) < EPS;
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
 * @param [in] coeff Array of equation coefficients
 * @param [out] root Pointer to the root
 * @return Number of roots
 * @note In case of infinit number of roots returns INF_ROOTS
*/

int SolveLinear(double* coeff, double* roots){
	if (IsZero(coeff[1])){
		return (IsZero(coeff[0])) ? INF_ROOTS : 0;
	}
	else{
		roots[0] = -coeff[0]/coeff[1];
		return 1;
	}
}

/**
 * @param [in] coeff Array of equation coefficients
 * @param [out] root1 Pointer to first root
 * @param [out] root2 Pointer to second root
 * @return Number of roots
 * @note In case of infinit number of roots returns INF_ROOTS
*/

int SolveQuadratic(double* coeff, double* roots){
	double d = coeff[1]*coeff[1] - 4*coeff[2]*coeff[0];
	if (IsZero(d)){
		roots[0] = -coeff[1] / (2*coeff[2]);
		return ONE_ROOT;
	}
	else if (d < 0){
		return NO_ROOTS;
	}
	else{
		double sqrt_d = sqrt(d);
		roots[0] = (-coeff[1] + sqrt_d) / (2 * coeff[2]);
		roots[1] = (-coeff[1] - sqrt_d) / (2 * coeff[2]);
		return TWO_ROOTS;
	}
}


int SolveEquation(double* coeff, size_t coeff_size, double* roots, size_t roots_size){
	assert(isfinite(coeff[2]));
	assert(isfinite(coeff[1]));
	assert(roots != NULL);
	assert(coeff_size >= MAX_COEFF_CNT);
	assert(roots_size >= MAX_ROOTS);

	if (!IsZero(coeff[2])){
		return SolveQuadratic(coeff, roots);
	}else{
		return SolveLinear(coeff, roots);
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
	int c = EOF;
	while ((c = getchar()) != '\n' && c != EOF);
}

enum InputStatus EnterCoefficients(double* coeff){
	assert(coeff != NULL);

	char sep1 = 0, sep2 = 0, sep3 = 0; // TODO: maybe fix
	int argument_cnt = scanf("%lf%c%lf%c%lf%c", &coeff[2], &sep1, &coeff[1], &sep2, &coeff[0], &sep3);
	if (argument_cnt != 6 || sep1 != ' ' || sep2 != ' ' || sep3 != '\n'){
		return INVALID_STR;
	}
	if (!CheckDouble(coeff[0]) || !CheckDouble(coeff[1]) || !CheckDouble(coeff[2])){
		return INVALID_NUM;
	}
	return VALID_INPUT;
}

void PrintRoots(int nRoots, double* roots, int digits_after_point){
	switch (nRoots){
		case NO_ROOTS:
			printf("No roots\n");
			break;
		case INF_ROOTS:
			printf("Infinity number of roots\n");
			break;
		case ONE_ROOT:
			printf("x = %.*lf\n", digits_after_point, roots[0]);
			break;
		case TWO_ROOTS:
			printf("x1 = %.*lf, x2 = %.*lf\n", digits_after_point, roots[0], digits_after_point, roots[1]);
			break;
		default:
			printf("Something went wrong...\n");
			break;
	}
	
}

int ProcessFlags(int argc, char* argv[], int* digits_after_point){
	if (argc>1){
		if (strcmp(argv[1], "-dap") == 0 && CheckStringOfDigits(argv[2])){ // maybe accuracy
			*digits_after_point = atoi(argv[2]);
		}else{
			ColoredPrintf(WHITE, RED, "Usage: qdr -dap digits_after_point\n");
			return -1;
		}
	}
	return 0;
}

void Greeting(){
	ColoredPrintf(CYAN, BLACK, "Quadratic Equation Solver\n ax\u00B2 + bx + c = 0"); //\u00B2 - unicode ² symbol
}

void PrintEquation(double* coeff, int digits_after_point){
	printf("You entered ");
	ColoredPrintf(GREEN, BLACK, "%.*lf", digits_after_point, coeff[2]);
	printf("x\u00B2%c", (coeff[1]>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", digits_after_point, coeff[1]);
	printf("x%c", (coeff[0]>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", digits_after_point, coeff[0]);
	printf(" = 0 equation.\n");
}

void RequestCoefficients(double* coeff, size_t coeff_size, int digits_after_point){
	assert(coeff_size >= MAX_COEFF_CNT);

	printf("\nEnter coefficients: ");
	enum InputStatus input_result = EnterCoefficients(coeff);
	while (input_result != VALID_INPUT){
		if (input_result == INVALID_NUM){
			ColoredPrintf(RED, NO_COLOR, "Invalid numbers. Don't use nan or inf\n");
			printf("Enter coefficients: ");
		}else if(input_result == INVALID_STR){
			ColoredPrintf(RED, NO_COLOR, "Invalid input. Please enter 3 numbers separated by space\n");
			printf("Enter coefficients: ");
			ClearStdinBuffer();
		}else{
			printf("Please try again.\n");
		}
		input_result = EnterCoefficients(coeff);
	}
	PrintEquation(coeff, digits_after_point);
}


void SortRoots(double* roots, size_t roots_size){
	if (roots_size < 2)return;
	if (roots[0] > roots[1]){
		double tmp = roots[1];
		roots[1] = roots[0];
		roots[0] = tmp;
	}
}

int TestSolveEquation(double* coeffs, size_t coeff_cnt, double* roots_ref, int nRootsRef){
	double roots[MAX_ROOTS];
	int nRoots = SolveEquation(coeffs, coeff_cnt, roots, MAX_ROOTS);
	SortRoots(roots, nRoots);
	SortRoots(roots_ref, nRootsRef);
	if (nRoots != nRootsRef || (nRoots == 1 && !Equals(roots[0], roots_ref[0]))
							|| (nRootsRef == 2 && (!Equals(roots[0], roots_ref[0]) 
												|| !Equals(roots[1], roots_ref[1])))){
		return 1;
	}
	return 0;
}

void RunTests(){

}

int main(int argc, char* argv[]){

	double testcfs[] = {0., 1., 4.};
	double testrts[] = {-4.};
	printf("Test 0 1 4 res: %d", TestSolveEquation(testcfs, 3, testrts, 1));

	int digits_after_point = 2;
	
	ProcessFlags(argc, argv, &digits_after_point);

	Greeting();

	double coeffs[MAX_COEFF_CNT] = {0., 0., 0.};

	RequestCoefficients(coeffs, MAX_COEFF_CNT, digits_after_point);

	double roots[MAX_ROOTS] = {0., 0.};
	int nRoots = SolveEquation(coeffs, MAX_COEFF_CNT, roots, MAX_ROOTS);

	PrintRoots(nRoots, roots, digits_after_point);

	return 0;
}
