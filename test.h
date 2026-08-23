#ifndef __TEST__
#define __TEST__

#include "solver.h"
#include <stddef.h>

const int RND_TEST_UPPER = 10;
const int RND_TEST_LOWER = -10;
const int DEFAULT_TEST_COUNT = 100;
const bool DEFAULT_NEED_TEST = false;

enum TestStatus{
	TEST_FAILED, 
	TEST_PASSED
};

const char* const TestErrorMessage = R"(
expected: %lf %lf
got:      %lf %lf
)";

//! Setup random seed (by time)
void SetupRandom();

//! Get random number in [lower; upper]
int GetRandInBounds(int lower, int upper);

/**
 * Test equation solver with one test
 * @param [in] coeefs coefficients
 * @param [in] rootsRef reference solution
 * @param [out] test_res result calculated by equation solver
 * @returns testing status
 */
TestStatus TestSolver(const struct EquationCoeffs* coeffs, const struct EquationSolutions* rootsRef, struct EquationSolutions* test_res);

/**
 * Generate array of random tests for solver
 * @param [out] test_coeffs array for write generated test coeffs
 * @param [out] test_res array for write generated test solutions
 */
void GenSolverTests(size_t test_cnt, struct EquationCoeffs* test_coeffs, struct EquationSolutions* test_res);

/**
 * Run multiple solver tests
 * @param [int] test_coeffs array of test coeffs
 * @param [int] test_res array of solutions
 */
void RunSolverTests(size_t test_cnt, const struct EquationCoeffs test_coeffs[], const struct EquationSolutions test_res[]);

/**
 * Print error message (test failed)
 * @param [int] coeffs test coeffs
 * @param [int] expected expected test solution
 * @param [int] expected got test solution 
 */
void PrintTestError(const struct EquationCoeffs* coeffs, const struct EquationSolutions* expected, const struct EquationSolutions* got);

//! Run all unit tests
void TestAll();

/**
 * Generate random coefficients by equation solutions
 * @param [in] test_res solutions
 * @returns equation coefficients
 */
struct EquationCoeffs GetCoeffsBySolution(const struct EquationSolutions* test_res);

#endif /*__TEST__*/