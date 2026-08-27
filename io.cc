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

//! Internal function for enter expression from stdin
enum InputStatus _EnterExpression(struct EquationCoeffs* coeffs);

void _PrintInvalidInputMessage(enum InputStatus input_result){
	if (input_result == INVALID_NUM){
			ColoredPrintf(RED, NO_COLOR, "Invalid numbers. Don't use nan or inf\n");
		}else if(input_result == INVALID_STR){
			ColoredPrintf(RED, NO_COLOR, "Invalid input. Try again\n");
		}else{
			printf("Please try again.\n");
	}
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

void RequestCoefficients(struct EquationCoeffs* coeffs, const char* message){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	printf("\n%s", message);
	enum InputStatus input_result = _EnterCoefficients(coeffs);
	while (input_result != VALID_INPUT){
		_PrintInvalidInputMessage(input_result);
		printf("%s", message);
		input_result = _EnterCoefficients(coeffs);
	}
}

void RequestExpression(struct EquationCoeffs* coeffs, const char* message){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	printf("\n%s", message);

	enum InputStatus input_result = _EnterExpression(coeffs);

	while (input_result != VALID_INPUT){
		_PrintInvalidInputMessage(input_result);
		printf("%s", message);
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