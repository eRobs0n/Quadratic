#include <cctype>
#include <cmath>
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

void ColoredPrintf(enum ConsoleColors text_color, enum ConsoleColors bg_color, const char* fmt, ...){
	char res_color[COLOR_STR_MAX_LEN];
	if (text_color == NO_COLOR){
		snprintf(res_color, COLOR_STR_MAX_LEN, "\033[4%dm", bg_color);
	}
	if (bg_color == NO_COLOR){
		snprintf(res_color, COLOR_STR_MAX_LEN, "\033[3%dm", text_color);
	}
	if (text_color != NO_COLOR && bg_color != NO_COLOR) {
		snprintf(res_color, COLOR_STR_MAX_LEN, "\033[3%d;4%dm", text_color, bg_color);
	}
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
 * @note In case of infinit number of roots returns INF_ROOTS
*/

int SolveLinear(double coeff_of_x, double free_coeff, double* root){
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
	double d = coeff_of_x*coeff_of_x - 4*coeff_of_square_x*free_coeff;
	if (IsZero(d)){
		*root1 = -coeff_of_x / (2*coeff_of_square_x);
		return ONE_ROOT;
	}
	else if (d < 0){
		return NO_ROOTS;
	}
	else{
		double sqrt_d = sqrt(d);
		*root1 = (-coeff_of_x + sqrt_d) / (2 * coeff_of_square_x);
		*root2 = (-coeff_of_x - sqrt_d) / (2 * coeff_of_square_x);
		return TWO_ROOTS;
	}
}


int SolveEquation(double coeff_of_square_x, double coeff_of_x, double free_coeff, double* root1, double* root2){
	assert(isfinite(coeff_of_square_x));
	assert(isfinite(coeff_of_x));
	assert(root1 != NULL);
	assert(root2 != NULL);
	assert(root1 != root2);

	if (!IsZero(coeff_of_square_x)){
		return SolveQuadratic(coeff_of_square_x, coeff_of_x, free_coeff, root1, root2);
	}else{
		return SolveLinear(coeff_of_x, free_coeff, root1);
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

void ProcessFlags(int argc, char* argv[], int* digits_after_point){
	if (argc>1){
		if (strcmp(argv[1], "-dap") == 0 && CheckStringOfDigits(argv[2])){
			*digits_after_point = atoi(argv[2]);
		}else{
			ColoredPrintf(WHITE, RED, "Usage: qdr -dap digits_after_point\n");
			exit(-1);
		}
	}
}

void Greeting(){
	ColoredPrintf(CYAN, BLACK, "Quadratic Equation Solver\n ax\u00B2 + bx + c = 0\n"); //\u00B2 - unicode ² symbol
}

void RequestCoefficients(double* a, double* b, double* c, int digits_after_point){
	printf("Enter coefficients: ");
	enum InputStatus input_result = EnterCoefficients(a, b, c);
	while (input_result != VALID_INPUT){
		if (input_result == INVALID_NUM){
			ColoredPrintf(RED, BLACK, "Invalid numbers. Don't use nan or inf\n");
			printf("Enter coefficients: ");
		}else if(input_result == INVALID_STR){
			ColoredPrintf(RED, BLACK, "Invalid input. Please enter 3 numbers separated by space\n");
			printf("Enter coefficients: ");
			ClearStdinBuffer();
		}else{
			printf("Please try again.\n");
		}
		input_result = EnterCoefficients(a, b, c);
	}

	ColoredPrintf(GREEN, BLACK, "You entered %.*lfx\u00B2 %c %.*lfx %c %.*lf = 0 equation.\n", 
		                     digits_after_point,      *a, 
		(*b>=0) ? '+' : '-', digits_after_point, fabs(*b), 
		(*c>=0) ? '+' : '-', digits_after_point, fabs(*c)
		); 
}

int main(int argc, char* argv[]){
	double a = 0., b = 0., c = 0.;

	int digits_after_point = 2;
	
	ProcessFlags(argc, argv, &digits_after_point);

	Greeting();

	RequestCoefficients(&a, &b, &c, digits_after_point);

	double x1 = 0., x2 = 0.;
	int nRoots = SolveEquation(a, b, c, &x1, &x2);

	PrintRoots(nRoots, x1, x2, digits_after_point);

	return 0;
}
