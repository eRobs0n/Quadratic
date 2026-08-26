#include "test.h"

#include <cstdio>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "common.h"
#include "d_math.h"
#include "io.h"
#include "parser.h"
#include "solver.h"
#include "stdio.h"
#include "string.h"
#include "parser.h"



const EquationCoeffs spec_solver_tests_coeffs[] = {
/*   x^2   x    c       */
	{ 0,   0,   0},
	{ 0,   4,  -8},
	{ 1,   0,  -4},
	{ 0,   0,   1},
	{ 0,   1,   0}
};

const EquationSolutions spec_solver_tests_solutions[] = {
	{.root1 = 0, .root2 =  0, .nRoots = INF_ROOTS},
	{.root1 = 0, .root2 =  2, .nRoots = ONE_ROOT},
	{.root1 = 2, .root2 = -2, .nRoots = TWO_ROOTS},
	{.root1 = 0, .root2 =  0, .nRoots = NO_ROOTS},
	{.root1 = 0, .root2 =  0, .nRoots = ONE_ROOT}
};
char* spec_parser_tests_strings[] = {
	"x^2+x+1", 
	"-x-x^2",
	"-42",
	"1.5x^2 - \t 1.5x \t -6.75",
	"1=1",
	"5x^2 = 4x - 12 + 32 - 1*x^2",
	"=52",
	"4x=2=2-5*x",
	"aboba worng string",
	"x*x+x^2+x=2=2=2",
	"x^2=+-x^2"
};

const EquationCoeffs spec_parser_tests_coeffs[] = {
/*   x^2     x      c   */
	{ 1,     1,     1},
	{ -1,   -1,     0},
	{  0,    0,   -42},
	{1.5,  -1.5, -6.75},
	{  0,    0,     0},
	{  6,   -4,    20},
	{  0,    0,     0},
	{  0,    0,     0},
	{  0,    0,     0},
	{  0,    0,     0},
	{  0,    0,     0}
};

const ParsingStatus spec_parser_tests_statuses[] = {
	PARSING_OK,
	PARSING_OK,
	PARSING_OK,
	PARSING_OK,
	PARSING_OK,
	PARSING_OK,
	PARSING_ERROR,
	PARSING_ERROR,
	PARSING_ERROR,
	PARSING_ERROR,
	PARSING_ERROR
};

const int _MAX_TERMS_FOR_PARSER_TEST = 20;

int GetRandInBounds(int lower, int upper){
	return lower + (rand() % (upper - lower + 1));
}

TestStatus TestSolver(const struct EquationCoeffs* coeffs, const struct EquationSolutions* rootsRef, struct EquationSolutions* test_res){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");
	assert(rootsRef != NULL);
	assert(test_res != NULL);

	SolveEquation(coeffs, test_res);
	if (!CompareSolutions(test_res, rootsRef)){
		return TEST_FAILED;
	}
	return TEST_PASSED;
}

void GenSolverTests(size_t test_cnt, struct EquationCoeffs* test_coeffs, struct EquationSolutions* test_res){
	assert(test_coeffs != NULL);
	assert(test_res != NULL);

	for (size_t i = 0; i < test_cnt; i++){
		test_res[i].nRoots = TWO_ROOTS;

		do{
			test_res[i].root1 = (double) GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
			test_res[i].root2 = (double) GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
		}while (Equals(test_res[i].root1, test_res[i].root2));

		test_coeffs[i] = GetCoeffsBySolution(&test_res[i]);
	}
}

void RunSolverTests(size_t test_cnt, const struct EquationCoeffs test_coeffs[], const struct EquationSolutions test_res[]){
	assert(test_coeffs != NULL);
	assert (test_res != NULL);

	for (size_t i = 0; i < test_cnt; i++){
		struct EquationSolutions result;
		if (TestSolver(&test_coeffs[i], &test_res[i], &result) == TEST_FAILED)
			PrintSolverTestError(&test_coeffs[i], &test_res[i], &result);
		else
			ColoredPrintf(GREEN, BLACK, "Test %d passed!\n", i+1);
	}
}

void SetupRandom(){
	srand(time(0));
}

void PrintSolverTestError(const struct EquationCoeffs* coeffs, const struct EquationSolutions* expected, const struct EquationSolutions* got){
	assert(coeffs != NULL && "Error! Pointer to coeffs is NULL");
	assert (expected != NULL);
	assert (got != NULL);

	ColoredPrintf(RED, NO_COLOR, "Solver test FAILED:\n");
		PrintEquation(coeffs, 2);	
		ColoredPrintf(RED, NO_COLOR, "expected: %lf %lf\ngot:      %lf %lf\n", expected->root1, expected->root2,
			got->root1, got->root2);
}

struct EquationCoeffs GetCoeffsBySolution(const struct EquationSolutions* test_res){
	assert(test_res != NULL);

	struct EquationCoeffs test_coeffs;
	int multiplier = GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
	while (multiplier == 0)
		multiplier = GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
	test_coeffs.coeff_of_sq_x = (double) multiplier;
	test_coeffs.coeff_of_x = -(test_res->root1 + test_res->root2) * (double) multiplier;
	test_coeffs.free_coeff = (double) multiplier * test_res->root1 * test_res->root2;
	return test_coeffs;
}

double _GenRandTerm(size_t term_length, char* term, enum TermCoefficient power){
	assert(term != NULL);
	assert(term_length <= MAX_TERM_LENGTH);

	double coeff = GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
	char t_str[MAX_TERM_LENGTH] = {0};

	int written_symbols = 0;

	switch(power){
	case CONSTANT_TERM:
		written_symbols = snprintf(t_str, term_length, "%+lf", coeff);
		break;
	case LINEAR_TERM:
		written_symbols = snprintf(t_str, term_length, "%+lf%s", 
			coeff, _LINEAR_VAR_STRINGS[GetRandInBounds(0, ARR_LEN(_LINEAR_VAR_STRINGS)-1)]);
		break;
	case LEADING_TERM:
		written_symbols = snprintf(t_str, term_length, "%+lf%s", 
			coeff, _LEADING_VAR_STRINGS[GetRandInBounds(0, ARR_LEN(_LEADING_VAR_STRINGS)-1)]);
		break;
	case INVALID_TERM:
		assert(false && "invalid term power in _GetRandTerm");
		break;
	case TERM_COEFFICIENT_MIN_VAL:
		assert(false && "invalid term power in _GetRandTerm");
		break;
	case TERM_COEFFICIENT_MAX_VAL:
		assert(false && "invalid term power in _GetRandTerm");
		break;
	default:
		assert(false && "invalid term power in _GetRandTerm");
		break;
	}

	if(written_symbols < 0){
		assert(false && "too small term_lingth in _GetRandTerm");
	}

	strncpy(term, t_str, term_length);
	return coeff;
}

void GenParserTests(size_t test_cnt, int max_test_string_len, char** test_strings, struct EquationCoeffs* test_res){
	assert(test_strings != NULL);
	assert(test_res != NULL);
	assert(MAX_TERM_LENGTH * _MAX_TERMS_FOR_PARSER_TEST < max_test_string_len && "Too small max_test_string_len!");

	for(unsigned i = 0; i < test_cnt; i++){
		struct EquationCoeffs coeff = {0,0,0};
		for(int j = 0; j < _MAX_TERMS_FOR_PARSER_TEST; j++){
			char rand_term[MAX_TERM_LENGTH] = {0};
	
			enum TermCoefficient t_type = (enum TermCoefficient) GetRandInBounds(CONSTANT_TERM, TERM_COEFFICIENT_MAX_VAL-1); 
			double one_cf = _GenRandTerm(MAX_TERM_LENGTH, rand_term, t_type);
			switch (t_type){
			case INVALID_TERM:
				assert(false && "Invalid enum in GenParserTests!");
				break;
			case LINEAR_TERM:
				coeff.coeff_of_x += one_cf;
				break;
			case CONSTANT_TERM:
				coeff.free_coeff += one_cf;
				break;
			case LEADING_TERM:
				coeff.coeff_of_sq_x += one_cf;
				break;
			case TERM_COEFFICIENT_MIN_VAL:
				assert(false && "invalid term power in GenParserTests");
				break;
			case TERM_COEFFICIENT_MAX_VAL:
				assert(false && "invalid term power in GenParserTests");
				break;
			default:
				assert(false && "Invalid enum in GenParserTests!");
			}

			strncat(test_strings[i], rand_term, MAX_TERM_LENGTH);
			test_res[i] = coeff;
		}
	}
}

TestStatus TestParser(const char* test_string, const EquationCoeffs* coeffsRef, enum ParsingStatus statRef,
	struct EquationCoeffs* test_res){
	assert(test_string != NULL);
	assert(coeffsRef != NULL);
	assert(test_res != NULL);

	enum ParsingStatus test_res_stat = ParseExpression(test_string, test_res);
	if (test_res_stat != statRef && !CompareCoeffs(test_res, coeffsRef)){
		return TEST_FAILED;
	}
	return TEST_PASSED;
}

void PrintParserTestError(const char* test_str, const struct EquationCoeffs* expected, const struct EquationCoeffs* got){
	assert(test_str != NULL);
	assert (expected != NULL);
	assert (got != NULL);

	ColoredPrintf(RED, NO_COLOR, "Parser test FAILED:\n"
		"Expected result: eq - ");
	PrintEquation(expected, DEFAULT_ACCURACY);
	ColoredPrintf(RED, NO_COLOR,
		"Got result: eq - ");
	PrintEquation(got, DEFAULT_ACCURACY);
	printf("\n");
	ColoredPrintf(RED, NO_COLOR, "Parserd string was %s\n", test_str);
}

void RunParserTests(size_t test_cnt, char** test_strings, const enum ParsingStatus* statsRef,
 const struct EquationCoeffs* test_res){
	assert(test_strings != NULL);
	assert(test_res != NULL);

	for(size_t i = 0; i < test_cnt; i++){
		struct EquationCoeffs result = {0., 0., 0.};
		if (TestParser(test_strings[i], &test_res[i], statsRef[i], &result) == TEST_FAILED){
			PrintParserTestError(test_strings[i], &test_res[i], &result);
		}else{
			ColoredPrintf(GREEN, BLACK, "Parser test %d passed!\n", i+1);
		}
	}
}


void TestAll(){
	ColoredPrintf(YELLOW, NO_COLOR, "Testing solver...\n");
	printf("Testing solver group 1 - random tests:\n");
	struct EquationCoeffs test_coeffs[DEFAULT_SOLVER_TEST_COUNT];
	struct EquationSolutions test_res[DEFAULT_SOLVER_TEST_COUNT];
	for(int i = 0; i < DEFAULT_SOLVER_TEST_COUNT; i++){
		InitEquationCoeffs(&test_coeffs[i]);
		InitEquationSolutions(&test_res[i]);
	}
	GenSolverTests(DEFAULT_SOLVER_TEST_COUNT, test_coeffs, test_res);
	RunSolverTests(DEFAULT_SOLVER_TEST_COUNT, test_coeffs, test_res);

	printf("Testing solver group 2 - specific tests:\n");
	RunSolverTests(ARR_LEN(spec_solver_tests_coeffs), spec_solver_tests_coeffs, spec_solver_tests_solutions);

	ColoredPrintf(YELLOW, NO_COLOR, "Testing parser...\n");
	printf("Testing parser group 1 - random tests:\n");

	char** test_strings = (char**) calloc(DEFAULT_PARSER_TEST_COUNT, sizeof(char*));
	for(int i = 0; i < DEFAULT_PARSER_TEST_COUNT; i++){
		test_strings[i] = (char*) calloc(MAX_EXPRESSION_LENGTH, sizeof(char));
	}
	struct EquationCoeffs parser_test_ref[DEFAULT_PARSER_TEST_COUNT] = {};
	enum ParsingStatus parser_test_ref_stats[DEFAULT_PARSER_TEST_COUNT] = {};
	for (int i = 0; i < DEFAULT_PARSER_TEST_COUNT; i++) parser_test_ref_stats[i] = PARSING_OK;

	GenParserTests(DEFAULT_PARSER_TEST_COUNT, MAX_EXPRESSION_LENGTH, test_strings, parser_test_ref);
	RunParserTests(DEFAULT_PARSER_TEST_COUNT, test_strings, parser_test_ref_stats, parser_test_ref);

	for(int i = 0; i < DEFAULT_PARSER_TEST_COUNT; i++){
		free(test_strings[i]);
	}
	free(test_strings);

	printf("Testing parser group 2 - specific tests:\n");
	RunParserTests(ARR_LEN(spec_parser_tests_strings), spec_parser_tests_strings, spec_parser_tests_statuses, spec_parser_tests_coeffs);

}


