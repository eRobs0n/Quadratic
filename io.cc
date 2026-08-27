#include "io.h"
#include "parser.h"
#include <cctype>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "d_math.h"
#include "parser.h"
#include "solver.h"

const char* const _CONSOLE_RESET = "\033[0m\033[K";

//! Internal function for enter expression from stdin
enum InputStatus _EnterExpression(struct EquationCoeffs* coeffs);

/**
 * Creates special string for console colored output
 * @param [out] res_color 
 * @param [in] res_color_len res_color string length
 * @param [in] text_color color of the text
 * @param [in] bg_color color of the background
 * @note Do not use!
 */
void _CreateColorStr(char* res_color, size_t res_color_len, enum ConsoleColors text_color, enum ConsoleColors bg_color);

void _PrintInvalidInputMessage(enum InputStatus input_result){
	if (input_result == INVALID_NUM){
			ColoredPrintf(RED, NO_COLOR, "Invalid numbers. Don't use nan or inf\n");
		}else if(input_result == INVALID_STR){
			ColoredPrintf(RED, NO_COLOR, "Invalid input. Try again\n");
		}else{
			printf("Please try again.\n");
	}
}

void _CreateColorStr(char* res_color, const size_t res_color_len, const enum ConsoleColors text_color, enum ConsoleColors bg_color){
	assert(res_color != NULL && "Error! Res color string is NULL");

	if (text_color != NO_COLOR && bg_color != NO_COLOR) {
		snprintf(res_color, res_color_len, "\033[3%d;4%dm", text_color, bg_color);
	}
	if (text_color == NO_COLOR){
		snprintf(res_color, res_color_len, "\033[4%dm", bg_color);
	}
	if (bg_color == NO_COLOR){
		snprintf(res_color, res_color_len, "\033[3%dm", text_color);
	}
}

void ColoredPrintf(enum ConsoleColors text_color, enum ConsoleColors bg_color, const char* fmt, ...){
	assert(fmt != NULL);

	char res_color[COLOR_STR_MAX_LEN];
	_CreateColorStr(res_color, COLOR_STR_MAX_LEN, text_color, bg_color);
	printf("%s", res_color);
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("%s", _CONSOLE_RESET);
}

bool CheckStringOfDigits(char *s){
	assert(s != NULL);

	char c;
	while ((c = *s++) != '\0'){
		if (!isdigit(c))
			return false;
	}
	return true;
}

void ClearStdinBuffer(){
	int c = EOF;
	while ((c = getchar()) != '\n' && c != EOF);
}

// Better send as pointer
void PrintRoots(const struct EquationSolutions* roots, int accuracy){
	assert(roots != NULL);


	switch (roots->nRoots){
		case NO_ROOTS:
			printf("No roots\n");
			break;
		case INF_ROOTS:
			printf("Infinity number of roots\n");
			break;
		case ONE_ROOT:
			printf("x = %.*lf\n", accuracy, roots->root1);
			break;
		case TWO_ROOTS:
			printf("x1 = %.*lf, x2 = %.*lf\n", accuracy, roots->root1, accuracy, roots->root2);
			break;
		default:
			printf("Something went wrong...\n");
			break;
	}
}

void Greeting(){
	ColoredPrintf(CYAN, BLACK, "Quadratic Equation Solver\n ax\u00B2 + bx + c = 0"); //\u00B2 - unicode ² symbol
}

void PrintEquation(const struct EquationCoeffs* coeffs, int accuracy){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	//printf("You entered ");
	ColoredPrintf(GREEN, BLACK, "%.*lf", accuracy, coeffs->coeff_of_sq_x);
	printf("x\u00B2%c", (coeffs->coeff_of_x>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", accuracy, coeffs->coeff_of_x);
	printf("x%c", (coeffs->free_coeff>=0) ? '+' : ' ');
	ColoredPrintf(GREEN, BLACK, "%.*lf", accuracy, coeffs->free_coeff);
	printf(" = 0\n");
	//printf(" = 0 equation.\n");
}

enum InputStatus _EnterCoefficients(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	char sep1 = 0, sep2 = 0, sep3 = 0;
	int argument_cnt = scanf("%lf%c %lf%c %lf%c", &coeffs->coeff_of_sq_x, &sep1, &coeffs->coeff_of_x, &sep2, &coeffs->free_coeff, &sep3);
	if (argument_cnt != 6 || !isspace(sep1) || !isspace(sep2) || sep3 != '\n'){
		ClearStdinBuffer();
		return INVALID_STR;
	}
	if (!CheckDouble(coeffs->free_coeff) || !CheckDouble(coeffs->coeff_of_x) || !CheckDouble(coeffs->coeff_of_sq_x)){
		return INVALID_NUM;
	}
	return VALID_INPUT;
}

YesNoInputStatus YesNoInput(){
	char c = 0;
	scanf("%c", &c);
	c = (char) tolower(c);
	if (c == 'y'){
		return YES_ANS;
	}else if(c == 'n'){
		return NO_ANS;
	}
	return INVALID_ANS;
}

void RequestCoefficients(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	printf("\nEnter coefficients: ");
	enum InputStatus input_result = _EnterCoefficients(coeffs);
	while (input_result != VALID_INPUT){
		_PrintInvalidInputMessage(input_result);
		printf("Enter coefficients: ");
		input_result = _EnterCoefficients(coeffs);
	}
}

void RequestExpression(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	printf("\nEnter expression like a*x^2 + b*x + c: ");

	enum InputStatus input_result = _EnterExpression(coeffs);

	while (input_result != VALID_INPUT){
		_PrintInvalidInputMessage(input_result);
		printf("Enter expression like a*x^2 + b*x + c: ");
		input_result = _EnterExpression(coeffs);
	}

}

//! Internal function for enter expression from stdin
enum InputStatus _EnterExpression(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	char expr[MAX_EXPRESSION_LENGTH] = {0};
	char* res = fgets(expr, MAX_EXPRESSION_LENGTH, stdin);

	if (res == NULL){
		return INVALID_STR;
	}

	enum ParsingStatus parsing_result = ParseExpression(expr, coeffs);

	if(parsing_result == PARSING_ERROR) {
		return INVALID_STR;
	}

	if (!CheckDouble(coeffs->coeff_of_sq_x) || 
		 !CheckDouble(coeffs->coeff_of_x)    || 
		 !CheckDouble(coeffs->free_coeff)){
		*coeffs = {0, 0, 0};
		return INVALID_NUM; 
	}

	return VALID_INPUT;
}








//-----------------------