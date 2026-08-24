#include "test.h"

#include <cstdio>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "common.h"
#include "io.h"
#include "parser.h"
#include "solver.h"
#include "stdio.h"
#include "string.h"
#include "parser.h"

const EquationCoeffs spec_solver_tests_coeffs[] = {
	{0, 0, 0},
	{0, 4, -8},
	{1, 0, -4},
	{0, 0, 1}
};

const EquationSolutions spec_solver_tests_solutions[] = {
	{0, 0, INF_ROOTS},
	{0, 2, ONE_ROOT},
	{2, -2, TWO_ROOTS},
	{0, 0, NO_ROOTS}
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

	ColoredPrintf(RED, NO_COLOR, "Test FAILED:\n");
		PrintEquation(coeffs, 2);	
		ColoredPrintf(RED, NO_COLOR, TestErrorMessage, expected->root1, expected->root2,
			got->root1, got->root2);
}

void TestAll(){
	ColoredPrintf(YELLOW, NO_COLOR, "Testing solver...\n");
	printf("Testing group 1 - random tests:\n");
	struct EquationCoeffs test_coeffs[DEFAULT_SOLVER_TEST_COUNT];
	struct EquationSolutions test_res[DEFAULT_SOLVER_TEST_COUNT];
	for(int i = 0; i<DEFAULT_SOLVER_TEST_COUNT; i++){
		InitEquationCoeffs(&test_coeffs[i]);
		InitEquationSolutions(&test_res[i]);
	}
	GenSolverTests(DEFAULT_SOLVER_TEST_COUNT, test_coeffs, test_res);
	RunSolverTests(DEFAULT_SOLVER_TEST_COUNT, test_coeffs, test_res);

	printf("Testing group 2 - specific tests:\n");
	RunSolverTests(ARR_LEN(spec_solver_tests_coeffs), spec_solver_tests_coeffs, spec_solver_tests_solutions);

	ColoredPrintf(YELLOW, NO_COLOR, "Testing parser...\n");
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

void _GenRandTerm(size_t term_length, char* term, enum TermCoefficient power){
	assert(term != NULL);
	assert(term_length < MAX_TERM_LENGTH);

	double coeff = GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
	char t_str[MAX_TERM_LENGTH] = {0};

	int written_symbols = 0;

	switch(power){
	case CONSTANT_TERM:
		written_symbols = snprintf(t_str, term_length, "%lf", coeff);
		break;
	case LINEAR_TERM:
		written_symbols = snprintf(t_str, term_length, "%lf%s", 
			coeff, _LINEAR_VAR_STRINGS[GetRandInBounds(0, ARR_LEN(_LINEAR_VAR_STRINGS))]);
		break;
	case LEADING_TERM:
		written_symbols = snprintf(t_str, term_length, "%lf%s", 
			coeff, _LEADING_VAR_STRINGS[GetRandInBounds(0, ARR_LEN(_LEADING_VAR_STRINGS))]);
		break;
	default:
		assert("invalid term power in _GetRandTerm");
	}

	if(written_symbols < 0){
		assert("too small term_lingth in _GetRandTerm");
	}

	strncpy(term, t_str, term_length);
}

void GenParserTests(size_t test_cnt, size_t max_test_string_len, char* test_strings[], struct EquationCoeffs* test_res){
	assert(test_strings != NULL);
	assert(test_res != NULL);
	assert(MAX_TERM_LENGTH * _MAX_TERMS_FOR_PARSER_TEST < max_test_string_len && "Too small max_test_string_len!");

	for(int i = 0; i < test_cnt; i++){
		for(int j = 0; j < _MAX_TERMS_FOR_PARSER_TEST; j++){
			char rand_term[MAX_TERM_LENGTH] = {0};
			enum TermCoefficient t_type = (enum TermCoefficient) GetRandInBounds(INVALID_TERM+1, TERM_COEFFICIENT_MAX_VAL-1); 
			_GenRandTerm(MAX_TERM_LENGTH, rand_term, t_type); //TODO maybe fix, bad enum cast
			strncat(test_strings[i], rand_term, MAX_TERM_LENGTH);
		}
	}
}

TestStatus TestParser(const char* test_string, const EquationCoeffs* coeffsRef, enum ParsingStatus statusRef, 
	struct EquationCoeffs* test_res, enum ParsingStatus* test_status){
	assert(test_string != NULL);
	assert(coeffsRef != NULL);
	assert(test_res != NULL);

	*test_status = ParseExpression(test_string, test_res);
	if (*test_status != statusRef){
		return TEST_FAILED;
	}
	if (!CompareCoeffs(test_res, coeffsRef)){
		return TEST_FAILED;
	}
	return TEST_PASSED;
}


