#ifndef __TEST__
#define __TEST__

#include "parser.h"
#include "solver.h"
#include <stddef.h>

const int RND_TEST_UPPER = 100;
const int RND_TEST_LOWER = -100;
const int DEFAULT_SOLVER_TEST_COUNT = 20;
const int DEFAULT_PARSER_TEST_COUNT = 20;
const bool DEFAULT_NEED_TEST = false;

enum TestStatus{
	TEST_FAILED, 
	TEST_PASSED
};

//TODO sovler testcase struct

struct ParserTestCase{
	const char* expression;
	struct EquationCoeffs parsingRes;
	enum ParsingStatus parsingStatus;
};


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
 * @param [in ] test_cnt Tests count
 * @param [out] test_coeffs array for write generated test coeffs
 * @param [out] test_res array for write generated test solutions
 */
void GenSolverTests(size_t test_cnt, struct EquationCoeffs* test_coeffs, struct EquationSolutions* test_res);

/**
 * Run multiple solver tests
 * @param [in] test_coeffs array of test coeffs
 * @param [in] test_res array of solutions
 */
void RunSolverTests(size_t test_cnt, const struct EquationCoeffs test_coeffs[], const struct EquationSolutions test_res[]);

/**
 * Print error message (test failed)
 * @param [in] coeffs test coeffs
 * @param [in] expected expected test solution
 * @param [in] expected got test solution 
 */
void PrintSolverTestError(const struct EquationCoeffs* coeffs, const struct EquationSolutions* expected, const struct EquationSolutions* got);

//! Run all unit tests
void TestAll();

/**
 * Generate random coefficients by equation solutions
 * @param [in] test_res solutions
 * @returns equation coefficients
 */
struct EquationCoeffs GetCoeffsBySolution(const struct EquationSolutions* test_res);

//! Gen random term string for parser
double _GenRandTerm(size_t term_length, char* term, enum TermCoefficient power);

/**
 * Generate array of random tests for parser
 * @param [in] Test count 
 * @param [in] max_test_string_len Max length for string that would be generated (usually MAX_EXPRESSION_LEN)
 * @param [out] test_strings Array of strings for testing parser
 * @param [out] test_res Array of test answers
 */
void GenParserTests(size_t test_cnt, int max_test_string_len, const char** test_strings, struct EquationCoeffs* test_res);

/**
 * Run tests for parser
 * @param [in] max_test_string_len Max length for string that would be generated (usually MAX_EXPRESSION_LEN)
 * @param [in] test_strings Array of strings for testing parser
 * @param [in] coeffsRef Array of test answers
 * @param [out] Result of parser work
 * @return Test status error or ok
 */
TestStatus TestParser(const char* test_string, const EquationCoeffs* coeffsRef, enum ParsingStatus statRef,
	struct EquationCoeffs* test_res);

/**
 * Run multiple parser tests
 * @param [in] test_cnt Size of test_strings and test_res arrays
 * @param [in] test_strings array of test strings
 * @param [in] statsRef array of test statuses that parser must return
 * @param [out] test_res array of parsing result
 */
void RunParserTests(size_t test_cnt, char** test_strings, const enum ParsingStatus* statsRef,
 const struct EquationCoeffs* test_res);

//! Prints parser test error message
void PrintParserTestError(const char* test_str, const struct EquationCoeffs* expected, const struct EquationCoeffs* got);


#endif /*__TEST__*/