#include "test.h"

#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "io.h"

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
			PrintTestError(&test_coeffs[i], &test_res[i], &result);
		else
			ColoredPrintf(GREEN, BLACK, "Test %d passed!\n", i+1);
	}
}

void SetupRandom(){
	srand(time(0));
}

void PrintTestError(const struct EquationCoeffs* coeffs, const struct EquationSolutions* expected, const struct EquationSolutions* got){
	assert(coeffs != NULL /*&& "Error! Pointer to coeffs is NULL"*/);
	assert (expected != NULL);
	assert (got != NULL);

	ColoredPrintf(RED, NO_COLOR, "Test FAILED:\n");
		PrintEquation(coeffs, 2);	
		ColoredPrintf(RED, NO_COLOR, TestErrorMessage, expected->root1, expected->root2,
			got->root1, got->root2);
}

void TestAll(){
	struct EquationCoeffs test_coeffs[DEFAULT_TEST_COUNT];
	struct EquationSolutions test_res[DEFAULT_TEST_COUNT];
	for(int i = 0; i<DEFAULT_TEST_COUNT; i++){
		InitEquationCoeffs(&test_coeffs[i]);
		InitEquationSolutions(&test_res[i]);
	}
	GenSolverTests(DEFAULT_TEST_COUNT, test_coeffs, test_res);
	RunSolverTests(DEFAULT_TEST_COUNT, test_coeffs, test_res);
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
