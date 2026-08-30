#ifndef __IO__
#define __IO__

#include <stddef.h>
#include "solver.h"
#include "parser.h"
#include "console.h"

const int DEFAULT_ACCURACY = 2;

enum InputStatus{
	VALID_INPUT,
	INVALID_STR,
	INVALID_NUM
};

enum YesNoInputStatus{
	INVALID_ANS,
	NO_ANS,
	YES_ANS
};

//! Checks if string contains only digits
bool CheckStringOfDigits(char *s);

//! Erase stdin buffer
void ClearStdinBuffer();

/**
 * @param [in] roots Pointer to the roots
 * @param [in] accuracy Output accuracy (digits after point)
*/
void PrintRoots(const struct EquationSolutions* roots, int accuracy);

//! Print greeting
void Greeting();

/**
 * @param [in] coeffs Pointer to the coeffs of equation
 * @param [in] accuracy Output accuracy (digits after point)
*/
void PrintEquation(const struct EquationCoeffs* coeffs, int accuracy);


//! Internal function for enter coefficients from stdin
enum InputStatus _EnterCoefficients(struct EquationCoeffs* coeffs);

/**
 * Ask user to enter equation coefficients (3 numbers)
 * @param [out] coeffs Pointer to the coeffs
 * @param [in] message Request message
*/
void RequestCoefficients(struct EquationCoeffs* coeffs, const char* message);

//! Request Yes or No (y/n) from console
YesNoInputStatus YesNoInput();

/**
 * Ask user to enter expression like ax^2+bx+c
 * @param [out] coeffs Pointer to the coeffs
 * @param [in] message Request message
*/
void RequestExpression(struct EquationCoeffs* coeffs, const char* message);

#endif /*__IO__*/