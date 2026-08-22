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

struct EquationCoeffs{
	double coeff_of_sq_x;
	double coeff_of_x;
	double free_coeff;
};
struct EquationSolutions{
	double root1;
	double root2;
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

enum EquationSolutionsNumber{
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

int SolveLinear(struct EquationCoeffs coeffs, struct EquationSolutions* roots){
	if (IsZero(coeffs.coeff_of_x)){
		return (IsZero(coeffs.free_coeff)) ? INF_ROOTS : 0;
	}
	else{
		roots->root1 = -coeffs.free_coeff/coeffs.coeff_of_x;
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

int SolveQuadratic(struct EquationCoeffs coeffs, struct EquationSolutions* roots){
	double d = coeffs.coeff_of_x*coeffs.coeff_of_x - 4*coeffs.coeff_of_sq_x*coeffs.free_coeff;
	if (IsZero(d)){
		roots->root1 = -coeffs.coeff_of_x / (2*coeffs.coeff_of_sq_x);
		return ONE_ROOT;
	}
	else if (d < 0){
		return NO_ROOTS;
	}
	else{
		double sqrt_d = sqrt(d);
		roots->root1 = (-coeffs.coeff_of_x + sqrt_d) / (2 * coeffs.coeff_of_sq_x);
		roots->root2 = (-coeffs.coeff_of_x - sqrt_d) / (2 * coeffs.coeff_of_sq_x);
		return TWO_ROOTS;
	}
}


int SolveEquation(struct EquationCoeffs coeffs, struct EquationSolutions* roots){
	assert(isfinite(coeffs.coeff_of_sq_x));
	assert(isfinite(coeffs.coeff_of_x));
	assert(isfinite(coeffs.free_coeff));
	assert(roots != NULL);

	if (!IsZero(coeffs.coeff_of_sq_x)){
		return SolveQuadratic(coeffs, roots);
	}else{
		return SolveLinear(coeffs, roots);
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

void PrintRoots(int nRoots, struct EquationSolutions roots, int digits_after_point){
	switch (nRoots){
		case NO_ROOTS:
			printf("No roots\n");
			break;
		case INF_ROOTS:
			printf("Infinity number of roots\n");
			break;
		case ONE_ROOT:
			printf("x = %.*lf\n", digits_after_point, roots.root1);
			break;
		case TWO_ROOTS:
			printf("x1 = %.*lf, x2 = %.*lf\n", digits_after_point, roots.root1, digits_after_point, roots.root2);
			break;
		default:
			printf("Something went wrong...\n");
			break;
	}
	
}

int ProcessFlags(int argc, char* argv[], int* digits_after_point){
	if (argc>1){
		if (strcmp(argv[1], "-accuracy") == 0 && CheckStringOfDigits(argv[2])){ // maybe accuracy
			*digits_after_point = atoi(argv[2]);
		}else{
			ColoredPrintf(WHITE, RED, "Usage: qdr -accuracy digits_after_point\n");
			return -1;
		}
	}
	return 0;
}

void Greeting(){
	ColoredPrintf(CYAN, BLACK, "Quadratic Equation Solver\n ax\u00B2 + bx + c = 0"); //\u00B2 - unicode ² symbol
}

void PrintEquation(struct EquationCoeffs coeffs, int digits_after_point){
	printf("You entered ");
	ColoredPrintf(GREEN, BLACK, "%.*lf", digits_after_point, coeffs.coeff_of_sq_x);
	printf("x\u00B2%c", (coeffs.coeff_of_x>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", digits_after_point, coeffs.coeff_of_x);
	printf("x%c", (coeffs.free_coeff>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", digits_after_point, coeffs.free_coeff);
	printf(" = 0 equation.\n");
}

enum InputStatus EnterCoefficients(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL);

	char sep1 = 0, sep2 = 0, sep3 = 0; // TODO: maybe fix
	int argument_cnt = scanf("%lf%c%lf%c%lf%c", &coeffs->coeff_of_sq_x, &sep1, &coeffs->coeff_of_x, &sep2, &coeffs->free_coeff, &sep3);
	if (argument_cnt != 6 || sep1 != ' ' || sep2 != ' ' || sep3 != '\n'){
		return INVALID_STR;
	}
	if (!CheckDouble(coeffs->free_coeff) || !CheckDouble(coeffs->coeff_of_x) || !CheckDouble(coeffs->coeff_of_sq_x)){
		return INVALID_NUM;
	}
	return VALID_INPUT;
}

void RequestCoefficients(struct EquationCoeffs* coeffs){
	printf("\nEnter coefficients: ");
	enum InputStatus input_result = EnterCoefficients(coeffs);
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
		input_result = EnterCoefficients(coeffs);
	}
}



int main(int argc, char* argv[]){

	int digits_after_point = 2;
	
	ProcessFlags(argc, argv, &digits_after_point);

	Greeting();

	struct EquationCoeffs coeffs;
	RequestCoefficients(&coeffs);

	PrintEquation(coeffs, digits_after_point);

	struct EquationSolutions roots;
	int nRoots = SolveEquation(coeffs, &roots);

	PrintRoots(nRoots, roots, digits_after_point);

	return 0;
}
