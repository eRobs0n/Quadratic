#define _DEBUG_

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
#include <time.h>
#include <stdarg.h>

const double EPS = 1e-9;

const int COLOR_STR_MAX_LEN = 100;
const char* CONSOLE_RESET = "\033[0m\033[K";

const int DEFAULT_ACCURACY = 2;
const int DEFAULT_TEST_COUNT = 100;
const bool DEFAULT_NEED_TEST = false;

const int RND_TEST_UPPER = 10;
const int RND_TEST_LOWER = -10;

enum ConsoleColors{
	NO_COLOR = -1,
	BLACK = 0,
	RED = 1,
	GREEN = 2,
	YELLOW = 3,
	BLUE = 4,
	MAGENTA = 5,
	CYAN = 6,
	WHITE = 7
};

enum EquationSolutionsNumber{
	INF_ROOTS = -1,
	NO_ROOTS = 0,
	ONE_ROOT = 1,
	TWO_ROOTS = 2
};

enum InputStatus{
	VALID_INPUT,
	INVALID_STR,
	INVALID_NUM
};

enum TestStatus{
	TEST_FAILED, 
	TEST_PASSED
};

struct EquationCoeffs{
	double coeff_of_sq_x;
	double coeff_of_x;
	double free_coeff;
};
struct EquationSolutions{
	double root1;
	double root2;
	EquationSolutionsNumber nRoots;
};

bool Equals(double a, double b){
	return fabs(a-b) < EPS;
}

bool CompareSolutions(struct EquationSolutions a, struct EquationSolutions b){
	if (a.nRoots != b.nRoots) return false;
	if (a.nRoots == 1) return Equals(a.root1, b.root1);
	if (a.nRoots == 2) return (Equals(a.root1, b.root1) && Equals(a.root2, b.root2)) || 
							  (Equals(a.root1, b.root2) && Equals(a.root2, b.root1));
	return true;
}

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
	return Equals(d, 0.);
}

bool CheckDouble(double d){
	return !isnan(d) && isfinite(d);
}

/**
 * @param [in] coeff Array of equation coefficients
 * @param [out] root Pointer to the root
 * @return Number of roots
 * @note In case of infinit number of roots returns INF_ROOTS
*/

void SolveLinear(struct EquationCoeffs coeffs, struct EquationSolutions* roots){
	if (IsZero(coeffs.coeff_of_x)){
		roots->nRoots = (IsZero(coeffs.free_coeff)) ? INF_ROOTS : NO_ROOTS;
	}
	else{
		roots->root1 = -coeffs.free_coeff/coeffs.coeff_of_x;
		roots->nRoots = ONE_ROOT;
	}
}

/**
 * @param [in] coeff Array of equation coefficients
 * @param [out] root1 Pointer to first root
 * @param [out] root2 Pointer to second root
 * @return Number of roots
 * @note In case of infinit number of roots returns INF_ROOTS
*/

void SolveQuadratic(struct EquationCoeffs coeffs, struct EquationSolutions* roots){
	double d = coeffs.coeff_of_x*coeffs.coeff_of_x - 4*coeffs.coeff_of_sq_x*coeffs.free_coeff;
	if (IsZero(d)){
		roots->root1 = -coeffs.coeff_of_x / (2*coeffs.coeff_of_sq_x);
		roots->nRoots =  ONE_ROOT;
	}
	else if (d < 0){
		roots->nRoots =  NO_ROOTS;
	}
	else{
		double sqrt_d = sqrt(d);
		roots->root1 = (-coeffs.coeff_of_x + sqrt_d) / (2 * coeffs.coeff_of_sq_x);
		roots->root2 = (-coeffs.coeff_of_x - sqrt_d) / (2 * coeffs.coeff_of_sq_x);
		roots->nRoots = TWO_ROOTS;
	}
}


void SolveEquation(struct EquationCoeffs coeffs, struct EquationSolutions* roots){
	assert(isfinite(coeffs.coeff_of_sq_x));
	assert(isfinite(coeffs.coeff_of_x));
	assert(isfinite(coeffs.free_coeff));
	assert(roots != NULL);

	if (!IsZero(coeffs.coeff_of_sq_x)){
		SolveQuadratic(coeffs, roots);
	}else{
		SolveLinear(coeffs, roots);
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

void PrintRoots(struct EquationSolutions roots, int accuracy){
	switch (roots.nRoots){
		case NO_ROOTS:
			printf("No roots\n");
			break;
		case INF_ROOTS:
			printf("Infinity number of roots\n");
			break;
		case ONE_ROOT:
			printf("x = %.*lf\n", accuracy, roots.root1);
			break;
		case TWO_ROOTS:
			printf("x1 = %.*lf, x2 = %.*lf\n", accuracy, roots.root1, accuracy, roots.root2);
			break;
		default:
			printf("Something went wrong...\n");
			break;
	}
}

int ProcessFlags(int argc, char* argv[], int* accuracy, bool* need_test){
	assert(accuracy != NULL);

	for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], "--accuracy") == 0 && i+1 < argc && CheckStringOfDigits(argv[i+1])){
			*accuracy = atoi(argv[i+1]);
			i+=1;
		}
		else if (strcmp(argv[i], "--test") == 0){ 
			*need_test = true;
		}else{
			//printf("%s\n", argv[i]);
			ColoredPrintf(WHITE, RED, "Usage: qdr --accuracy *accuracy* --test\n");
			return -1;
		}
	}
	return 0;
}

void Greeting(){
	ColoredPrintf(CYAN, BLACK, "Quadratic Equation Solver\n ax\u00B2 + bx + c = 0"); //\u00B2 - unicode ² symbol
}

void PrintEquation(struct EquationCoeffs coeffs, int accuracy){
	//printf("You entered ");
	ColoredPrintf(GREEN, BLACK, "%.*lf", accuracy, coeffs.coeff_of_sq_x);
	printf("x\u00B2%c", (coeffs.coeff_of_x>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", accuracy, coeffs.coeff_of_x);
	printf("x%c", (coeffs.free_coeff>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", accuracy, coeffs.free_coeff);
	printf(" = 0\n");
	//printf(" = 0 equation.\n");
}

enum InputStatus EnterCoefficients(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL);

	char sep1 = 0, sep2 = 0, sep3 = 0; // TODO: maybe fix
	int argument_cnt = scanf("%lf%c %lf%c %lf%c", &coeffs->coeff_of_sq_x, &sep1, &coeffs->coeff_of_x, &sep2, &coeffs->free_coeff, &sep3);
	if (argument_cnt != 6 || !isspace(sep1) || !isspace(sep2) || sep3 != '\n'){
		return INVALID_STR;
	}
	if (!CheckDouble(coeffs->free_coeff) || !CheckDouble(coeffs->coeff_of_x) || !CheckDouble(coeffs->coeff_of_sq_x)){
		return INVALID_NUM;
	}
	return VALID_INPUT;
}

void RequestCoefficients(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL);

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

void PrintTestError(struct EquationCoeffs coeffs, struct EquationSolutions expected, struct EquationSolutions got){
	ColoredPrintf(RED, NO_COLOR, "Test FAILED:\n");
		PrintEquation(coeffs, 2);	
		ColoredPrintf(RED, NO_COLOR, "expected: %lf %lf\n"
									 "got:      %lf %lf\n", expected.root1, expected.root2,
			got.root1, got.root2);
}

TestStatus TestSolver(struct EquationCoeffs coeffs, struct EquationSolutions rootsRef, struct EquationSolutions* test_res){
	SolveEquation(coeffs, test_res);
	if (!CompareSolutions(*test_res, rootsRef)){
		return TEST_FAILED;
	}
	return TEST_PASSED;
}

int GetRandInBounds(int lower, int upper){
	return lower + (rand() % (upper - lower + 1));
}

struct EquationCoeffs GetCoeffsBySolution(struct EquationSolutions test_res){
	struct EquationCoeffs test_coeffs;
	int multiplier = GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
	while (multiplier == 0) multiplier = GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
	test_coeffs.coeff_of_sq_x = (double) multiplier;
	test_coeffs.coeff_of_x = -(test_res.root1 + test_res.root2) * (double) multiplier;
	test_coeffs.free_coeff = (double) multiplier * test_res.root1 * test_res.root2;
	return test_coeffs;
}

void GenTests(size_t test_cnt, struct EquationCoeffs* test_coeffs, struct EquationSolutions* test_res){
	assert(test_coeffs != NULL);
	assert(test_res != NULL);

	for (size_t i = 0; i < test_cnt; i++){
		test_res[i].nRoots = TWO_ROOTS;

		do{
			test_res[i].root1 = (double) GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
			test_res[i].root2 = (double) GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
		}while (Equals(test_res[i].root1, test_res[i].root2));

		test_coeffs[i] = GetCoeffsBySolution(test_res[i]);
		
	}
}

void RunTests(size_t test_cnt, struct EquationCoeffs test_coeffs[], struct EquationSolutions test_res[]){
	for (size_t i = 0; i < test_cnt; i++){
		struct EquationSolutions result;
		if (TestSolver(test_coeffs[i], test_res[i], &result) == TEST_FAILED) PrintTestError(test_coeffs[i], test_res[i], result);
	}
}

void SetupRandom(){
	srand(time(0));
}

void TestAll(){
	struct EquationCoeffs test_coeffs[DEFAULT_TEST_COUNT];
	struct EquationSolutions test_res[DEFAULT_TEST_COUNT];
	for(int i = 0; i<DEFAULT_TEST_COUNT; i++){
		test_coeffs[i] = {0. ,0. ,0.};
		test_res[i] = {0. ,0. ,NO_ROOTS};
	}
	GenTests(DEFAULT_TEST_COUNT, test_coeffs, test_res);
	RunTests(DEFAULT_TEST_COUNT, test_coeffs, test_res);
}

int main(int argc, char* argv[]){
	//SetupRandom();

	int accuracy = DEFAULT_ACCURACY;
	bool need_test = DEFAULT_NEED_TEST;

	ProcessFlags(argc, argv, &accuracy, &need_test);

	if (need_test){
		TestAll();
	}

	Greeting();

	struct EquationCoeffs coeffs;
	RequestCoefficients(&coeffs);

	PrintEquation(coeffs, accuracy);

	struct EquationSolutions roots;
	SolveEquation(coeffs, &roots);

	PrintRoots(roots, accuracy);

	return 0;
}
