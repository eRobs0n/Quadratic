#include "io.h"
#include "solver.h"
#include "cmd_flags.h"
#include "test.h"
#include "ai.h"
#include "plot.h"

//! Run manual mode, user must enter coefficients
void RunManualMode(int accuracy){
	Greeting();
	
	struct EquationCoeffs coeffs;
	RequestCoefficients(&coeffs, "Enter coefficients: ");

	PrintEquation(&coeffs, accuracy);

	struct EquationSolutions roots;
	SolveEquation(&coeffs, &roots);

	PrintRoots(&roots, accuracy);
}

//! Run expression mode, user must enter expression
void RunExpressionMode(int accuracy){
	Greeting();
	
	struct EquationCoeffs coeffs;
	RequestExpression(&coeffs, "Enter expression like a*x^2 + b*x + c: ");

	PrintEquation(&coeffs, accuracy);

	struct EquationSolutions roots;
	SolveEquation(&coeffs, &roots);

	PrintRoots(&roots, accuracy);

	GraphMode();
}

int main(int argc, char* argv[]){

	int accuracy = DEFAULT_ACCURACY;
	bool need_test = DEFAULT_NEED_TEST;
	bool is_ai = false;
	bool is_nogui = false;
	int random_test_seed = 0;

	int res = ProcessFlags(argc, argv, &accuracy, &need_test, &is_ai, &random_test_seed, &is_nogui);
	if (res == -1) return 0;
	
	if (need_test){
		TestAll(random_test_seed);
		return 0;
	}

	if (!is_nogui){
		GraphMode();
		return 0;
	}

	if (is_ai)
		AiMode();
	else;
		RunExpressionMode(accuracy);

	return 0;
}
