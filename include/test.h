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

struct SolverTestCase{
	struct EquationCoeffs coeffs;			//<Coefficeints of equation
	struct EquationSolutions refRoots;		//<Expected result
};

struct ParserTestCase{
	char* expression;						//<Expression for parser
	struct EquationCoeffs refCoeffs;		//<Expected parsing result
	enum ParsingStatus refStatus;			//<Expected parsing status
};


//! Setup random seed (if seed == 0 seed = time)
void SetupRandom(int seed);

//! Get random number in [lower; upper]
int GetRandInBounds(int lower, int upper);

/**
 * Test equation solver with one test
 * @param [in] test test struct
 * @param [out] ret_roots result calculated by equation solver
 * @returns testing status (OK or ERROR)
 */
TestStatus TestSolver(const struct SolverTestCase* test, struct EquationSolutions* ret_roots);

/**
 * Generate array of random tests for solver
 * @param [in] test_cnt test count
 * @param [out] tests array for writing generated random tests 
 */
void GenSolverTests(size_t test_cnt, struct SolverTestCase* tests);

/**
 * Run multiple solver tests
 * @param [in] test_cnt test count
 * @param [out] tests array of tests
 */
void RunSolverTests(size_t test_cnt, const struct SolverTestCase* tests);

/**
 * Print error message (test failed)
 * @param [in] test test data
 * @param [in] got result by solver
 */
void PrintSolverTestError(const struct SolverTestCase* test, const struct EquationSolutions *got);

//! Run all unit tests
void TestAll(int seed);

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
 * @param [in] test_cnt Test count 
 * @param [out] tests array for writing random parser tests
 */
void GenParserTests(size_t test_cnt, struct ParserTestCase* tests);

/**
 * Run one test for parser
 * @param [in] test Test data
 * @param [out] ret_coeffs pointer for write coeffs returned by parser
 * @param [out] ret_status pointer for write status returned by parser
 * @return Test status error or ok
 */
TestStatus TestParser(struct ParserTestCase* test, 
	struct EquationCoeffs* ret_coeffs, enum ParsingStatus* ret_status);

/**
 * Run multiple parser tests
 * @param [in] test_cnt Tests count
 * @param [in] tests Tests array
 */
void RunParserTests(size_t test_cnt, struct ParserTestCase* tests);

//! Prints parser test error message
void PrintParserTestError(struct ParserTestCase* test, struct EquationCoeffs* gotCoeffs, enum ParsingStatus gotStatus);


#endif /*__TEST__*/