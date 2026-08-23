#include "io.h"
#include "solver.h"
#include "cmd_flags.h"
#include "test.h"

void RunInteractiveMode(int accuracy){
	Greeting();

	struct EquationCoeffs coeffs;
	RequestCoefficients(&coeffs);

	PrintEquation(&coeffs, accuracy);

	struct EquationSolutions roots;
	SolveEquation(&coeffs, &roots);

	PrintRoots(&roots, accuracy);
}

int main(int argc, char* argv[]){
	SetupRandom();

	int accuracy = DEFAULT_ACCURACY;
	bool need_test = DEFAULT_NEED_TEST;

	ProcessFlags(argc, argv, &accuracy, &need_test);

	if (need_test){
		TestAll();
	} else {
		RunInteractiveMode(accuracy);
	}
	
	return 0;
}
