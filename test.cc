#include "test.h"

#include <cmath>
#include <cstdio>
#include <cstring>
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

struct SolverTestCase spec_solver_tests[] = {
//.    a*x^2    b*x  	c      root1  root2     nRoots
	{ {0, 		0, 	 	0}, 	{0, 	0,		INF_ROOTS}},
	{ {0, 		4, 	 	-8}, 	{0, 	2,		ONE_ROOT}},
	{ {1, 		0, 	 	-4}, 	{2, 	-2,		TWO_ROOTS}},
	{ {0, 		0, 	 	1}, 	{0, 	0,		NO_ROOTS}},
	{ {0, 		1, 	 	0}, 	{0, 	0,		ONE_ROOT}}

};

struct ParserTestCase spec_parser_tests[] = {
	{"x^2+x+1", 					{ 1, 1, 1}, 			PARSING_OK},
	{"-x-x^2", 	  					{ -1, -1, 0}, 			PARSING_OK},
	{"-42", 	    				{ 0, 0, -42}, 			PARSING_OK},
	{"1.5x^2 - \t 1.5x \t -6.75", 	{1.5,  -1.5, -6.75}, 	PARSING_OK},
	{"1=1", 						{ 0, 0, 0}, 			PARSING_OK},
	{"5x^2 = 4x - 12 + 32 - 1*x^2", { 6, -4, 20}, 			PARSING_OK},
	{"=52", 						{ NAN, NAN, NAN}, 		PARSING_ERROR},
	{"4x=2=2-5*x",   				{ NAN, NAN, NAN},       PARSING_ERROR},
	{"aboba worng string",			{ NAN, NAN, NAN},		PARSING_ERROR},
	{"x*x+x^2+x=2=2=2",				{ NAN, NAN, NAN},		PARSING_ERROR},
	{"x^2=+-x^2",					{ NAN, NAN, NAN},		PARSING_ERROR}
};

const int _MAX_TERMS_FOR_PARSER_TEST = 20;

int GetRandInBounds(int lower, int upper){
	return lower + (rand() % (upper - lower + 1));
}

TestStatus TestSolver(const struct SolverTestCase* test, struct EquationSolutions* ret_roots){
	assert(test != NULL);
	assert(ret_roots != NULL);

	SolveEquation(&test->coeffs, ret_roots);
	if (!CompareSolutions(&test->refRoots, ret_roots)){
		return TEST_FAILED;
	}
	return TEST_PASSED;
}


void GenSolverTests(size_t test_cnt, struct SolverTestCase* tests){
	assert(tests != NULL);

	for (size_t i = 0; i < test_cnt; i++){
		tests[i].refRoots.nRoots = TWO_ROOTS;

		do{
			tests[i].refRoots.root1 = (double) GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
			tests[i].refRoots.root2 = (double) GetRandInBounds(RND_TEST_LOWER, RND_TEST_UPPER);
		}while (Equals(tests[i].refRoots.root1, tests[i].refRoots.root2));

		tests[i].coeffs = GetCoeffsBySolution(&tests[i].refRoots);
	}
}

void RunSolverTests(size_t test_cnt, const struct SolverTestCase* tests){
	assert(tests != NULL);

	for (size_t i = 0; i < test_cnt; i++){
		struct EquationSolutions result = {};
		if (TestSolver(&tests[i], &result) == TEST_FAILED)
			PrintSolverTestError(&tests[i], &result);
		else
			ColoredPrintf(GREEN, BLACK, "Test %d passed!\n", i+1);
	}
}

void SetupRandom(){
	srand(time(0));
}

void PrintSolverTestError(const struct SolverTestCase* test, const struct EquationSolutions *got){
	assert(test != NULL);
	assert(got  != NULL);

	ColoredPrintf(RED, NO_COLOR, "Solver test FAILED:\n");
		PrintEquation(&test->coeffs, 2);	
		ColoredPrintf(RED, NO_COLOR, "expected: %lf %lf\ngot:      %lf %lf\n", test->refRoots.root1, test->refRoots.root2,
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

void GenParserTests(size_t test_cnt, struct ParserTestCase* tests){
	assert(tests != NULL);

	for(unsigned i = 0; i < test_cnt; i++){
		assert(tests[i].expression != NULL);

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

			//if (strlen(tests[i].expression) + strlen(rand_term) >= MAX_EXPRESSION_LENGTH) break;
			//debug_printf("%zu %zu\n\n", strlen(tests->expression), strlen(rand_term));

			strncat(tests[i].expression, rand_term, MAX_TERM_LENGTH);
			tests[i].refCoeffs = coeff;
			tests[i].refStatus = PARSING_OK;
		}
	}
}

TestStatus TestParser(struct ParserTestCase* test, 
	struct EquationCoeffs* ret_coeffs, enum ParsingStatus* ret_status){
	assert(test != NULL);
	assert(test->expression != NULL);
	assert(ret_coeffs != NULL);
	assert(ret_status != NULL);

	*ret_status = ParseExpression(test->expression, ret_coeffs);
	if (*ret_status != test->refStatus && !CompareCoeffs(&test->refCoeffs, ret_coeffs)){
		return TEST_FAILED;
	}
	return TEST_PASSED;
}

void PrintParserTestError(struct ParserTestCase* test, struct EquationCoeffs* gotCoeffs, enum ParsingStatus gotStatus){
	assert(test != NULL);
	assert (gotCoeffs != NULL);
	assert (test->expression != NULL);

	ColoredPrintf(RED, NO_COLOR, "Parser test FAILED:\n"
		"Expected result: status - %s, eq - ", test->refStatus == PARSING_ERROR ? "ERROR" : "OK");
	PrintEquation(&test->refCoeffs, DEFAULT_ACCURACY);
	ColoredPrintf(RED, NO_COLOR,
		"Got result: status - %s, eq - ", gotStatus == PARSING_ERROR ? "ERROR" : "OK");
	PrintEquation(gotCoeffs, DEFAULT_ACCURACY);
	ColoredPrintf(RED, NO_COLOR, "Parserd string was %s\n", test->expression);
}


void RunParserTests(size_t test_cnt, struct ParserTestCase* tests){
	assert(test != NULL);
	assert(test->expression != NULL);

	for(size_t i = 0; i < test_cnt; i++){
		struct EquationCoeffs result = {0., 0., 0.};
		enum ParsingStatus resultStatus;
		if (TestParser(test, &result, &resultStatus) != TEST_PASSED){
			PrintParserTestError(test, &result, resultStatus);
		}else{
			ColoredPrintf(GREEN, BLACK, "Parser test %d passed!\n", i+1);
		}
	}
}

void TestAll(){
	ColoredPrintf(YELLOW, NO_COLOR, "Testing solver...\n");
	printf("Testing solver group 1 - random tests:\n");
	
	struct SolverTestCase solver_rand_tests[DEFAULT_SOLVER_TEST_COUNT] = {};
	
	GenSolverTests(DEFAULT_SOLVER_TEST_COUNT, solver_rand_tests);
	RunSolverTests(DEFAULT_SOLVER_TEST_COUNT, solver_rand_tests);

	printf("Testing solver group 2 - specific tests:\n");
	RunSolverTests(ARR_LEN(spec_solver_tests), spec_solver_tests);

	//---------
	ColoredPrintf(YELLOW, NO_COLOR, "Testing parser...\n");
	printf("Testing parser group 1 - random tests:\n");


	struct ParserTestCase* parser_rand_tests = (struct ParserTestCase*) calloc(DEFAULT_PARSER_TEST_COUNT, 
		sizeof(struct ParserTestCase));

	assert(parser_rand_tests != NULL && "Can't allocate memory for parser random tests");

	for(int i = 0; i < DEFAULT_PARSER_TEST_COUNT; i++){
		parser_rand_tests[i].expression = (char*) calloc(MAX_EXPRESSION_LENGTH, sizeof(char));
		assert(parser_rand_tests[i].expression != NULL && "Can't allocate memory for parser random tests");
	}

	GenParserTests(DEFAULT_PARSER_TEST_COUNT, parser_rand_tests);
	RunParserTests(DEFAULT_PARSER_TEST_COUNT, parser_rand_tests);

	for(int i = 0; i < DEFAULT_PARSER_TEST_COUNT; i++){
		free(parser_rand_tests[i].expression);
	}
	free(parser_rand_tests);

	printf("Testing parser group 2 - specific tests:\n");
	RunParserTests(ARR_LEN(spec_parser_tests), spec_parser_tests);
}

