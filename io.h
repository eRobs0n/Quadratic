#ifndef __IO__
#define __IO__

#include <stddef.h>
#include "solver.h"

const int DEFAULT_ACCURACY = 2;
const int COLOR_STR_MAX_LEN = 100;
const char* const _CONSOLE_RESET = "\033[0m\033[K";

enum ConsoleColors{
	NO_COLOR = -1, ///< Default console color
	BLACK =     0,
	RED =       1,
	GREEN =     2,
	YELLOW =    3,
	BLUE =      4,
	MAGENTA =   5,
	CYAN =      6,
	WHITE =     7
};

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



/**
 * Creates special string for console colored output
 * @param [out] res_color 
 * @param [in] res_color_len res_color string length
 * @param [in] text_color color of the text
 * @param [in] bg_color color of the background
 * @note Do not use!
 */
void _CreateColorStr(char* res_color, size_t res_color_len, enum ConsoleColors text_color, enum ConsoleColors bg_color);

/**
 * Printf with custom background and text colors
 * @param [in] text_color color of the text
 * @param [in] bg_color color of the background
 * @param [in] fmt printf format string
 * @param [in] ... printf args
 * @note Only for MacOs zsh!
 */
void ColoredPrintf(enum ConsoleColors text_color, enum ConsoleColors bg_color, const char* fmt, ...);

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

enum InputStatus _EnterCoefficients(struct EquationCoeffs* coeffs);

/**
 * Ask user to enter equation coefficients (3 numbers)
 * @param [out] coeffs Pointer to the coeffs
*/
void RequestCoefficients(struct EquationCoeffs* coeffs);

//!Request Yes or No (y/n) from console
YesNoInputStatus YesNoInput();

//! Funny AI trolling greeting
void InterractiveAiGrreting();

//! Funny AI trolloing goodby
void InterractiveAiGoodbye();
#endif /*__IO__*/