#include "io.h"
#include <cctype>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "d_math.h"
#include "solver.h"

const char* _LEADING_VAR_STRINGS[] = {
	"x^2", 
	"x*x",
	"x**2",
	"*x^2", 
	"*x*x",
	"*x**2"
};

const char* _LINEAR_VAR_STRINGS[] = {
	"x",
	"*x"
};


void _PrintInvalidInputMessage(enum InputStatus input_result){
	if (input_result == INVALID_NUM){
			ColoredPrintf(RED, NO_COLOR, "Invalid numbers. Don't use nan or inf\n");
			printf("Enter coefficients: ");
		}else if(input_result == INVALID_STR){
			ColoredPrintf(RED, NO_COLOR, "Invalid input. Please enter 3 numbers separated by space\n");
			printf("Enter coefficients: ");
			ClearStdinBuffer();
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

	char sep1 = 0, sep2 = 0, sep3 = 0; // TODO: maybe fix
	int argument_cnt = scanf("%lf%c %lf%c %lf%c", &coeffs->coeff_of_sq_x, &sep1, &coeffs->coeff_of_x, &sep2, &coeffs->free_coeff, &sep3);
	if (argument_cnt != 6 || !isspace(sep1) || !isspace(sep2) || sep3 != '\n'){
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
	c = tolower(c);
	if (c == 'y'){
		return YES_ANS;
	}else if(c == 'n'){
		return NO_ANS;
	}
	return INVALID_ANS;
}

void StrSqueeze(char s[], int c){
   int i, j;
   for(i = j = 0; s[i] != '\0'; i++){
      if (s[i] != c)
         s[j++] = s[i];
   }
   s[j] = '\0';
}

char* SepByTerms(char* str, char* term, size_t max_term_length){
   if (*str == '\0') return NULL;
   char c;
   int pos = 0;
   while ((c = *str) != '\0'){
      if (pos!=0 && (c == '+' || c == '-')){
         return str;
      }
      if (pos>=max_term_length) return NULL;
      term[pos] = c;
      pos+=1;
      str++;
   }
   return str;
}


void RequestCoefficients(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	printf("\nEnter coefficients: ");
	enum InputStatus input_result = _EnterCoefficients(coeffs);
	while (input_result != VALID_INPUT){
		_PrintInvalidInputMessage(input_result);
		input_result = _EnterCoefficients(coeffs);
	}
}

//TODO перенести в RequestCoefficients
void RequestExpression(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	printf("\nEnter expression like a*x^2 + b*x + c: ");

	enum InputStatus input_result = _EnterExpression(coeffs);

	while (input_result != VALID_INPUT){
		_PrintInvalidInputMessage(input_result);
		input_result = _EnterExpression(coeffs);
	}

}

enum InputStatus _EnterExpression(struct EquationCoeffs* coeffs){
	assert(coeffs != NULL && "Error! coeffs pointer is NULL");

	char expr[MAX_EXPRESSION_LENGTH] = {0};
	char* res = fgets(expr, MAX_EXPRESSION_LENGTH, stdin);

	if (res == NULL){
		return INVALID_STR;
	}

	#ifdef _DEBUG
		printf("Got expression %s\n", res);
	#endif

	StrSqueeze(expr, ' ');

	#ifdef _DEBUG
		printf("After reoving whitespaces expression is %s\n", res);
	#endif

	char* token_ptr = expr;
	char term[MAX_TERM_LENGTH] = {0};
	int terms_cnt = 0;

	struct EquationCoeffs calc_coeffs = 
		{.coeff_of_sq_x = 0., .coeff_of_x = 0., .free_coeff = 0};

	while ((token_ptr = SepByTerms(token_ptr, term, MAX_TERM_LENGTH)) != NULL){
		#ifdef _DEBUG
			printf("Separated term %s\n", term);
		#endif

		double pred_coeff = 0.;
		enum TermCoefficient type = _ProcessTerm(term, &pred_coeff);

		#ifdef _DEBUG
			printf("Extracted coefficeint %lf\n", pred_coeff);
		#endif

		if (!CheckDouble(pred_coeff)){
			return INVALID_NUM;
		}

		#ifdef _DEBUG
			printf("Returned status: %d\n", type);
		#endif

		switch(type){
		case INVALID_TERM:
			return INVALID_STR;
			break;
		case CONSTANT_TERM:
			calc_coeffs.free_coeff += pred_coeff;
			break;
		case LINEAR_TERM:
			calc_coeffs.coeff_of_x += pred_coeff;
			break;
		case LEADING_TERM:
			calc_coeffs.coeff_of_sq_x += pred_coeff;
			break;
		default:
			assert(false);
		}

		memset(term, 0, MAX_TERM_LENGTH*sizeof(char));
	}

	*coeffs = calc_coeffs;

	return VALID_INPUT;
}

enum TermCoefficient _ProcessTerm(/*const*/ char* t, double* coeff){
	#ifdef _DEBUG
	printf("Runned _ProcessTerm with term %s and %p pointer to cf\n", t, coeff);
	#endif

	assert(t != NULL); //What?
	assert(coeff != NULL);

	double t_coeff = 0.;
	// if (atof(t)){ // TODO Fix! Not working

	// 	#ifdef _DEBUG
	// 		printf("Converted double - %s %lf\n", t, atof(t));
	// 	#endif

	// 	sscanf(t, "%lf", &t_coeff);
	// 	*coeff = t_coeff;
	// 	return CONSTANT_TERM;
	// }
	char var[MAX_TERM_LENGTH];
	sscanf(t, "%lf%s", &t_coeff, var);

	if (strlen(var) == 0){
		*coeff = t_coeff;
		return CONSTANT_TERM;
	}

	for (int i = 0; i < ARR_LEN(_LEADING_VAR_STRINGS); i++){
		if (strcmp(var, _LEADING_VAR_STRINGS[i]) == 0){
			*coeff = t_coeff;
			return LEADING_TERM;
		}
	}

	for (int i = 0; i < ARR_LEN(_LINEAR_VAR_STRINGS); i++){
		if (strcmp(var, _LINEAR_VAR_STRINGS[i]) == 0){
			*coeff = t_coeff;
			return LINEAR_TERM;
		}
	}

	return INVALID_TERM;
}










//-----------------------

const char* const _funnyAiGreeting = R"(
🧠✨ Уважаемый пользователь.

Перед вами не просто квадратное уравнение.

Перед вами — **возможность**.

**x² − 4x + 3 = 0**

Два корня.
Один дискриминант.
Бесконечное количество способов сказать, что это сделала нейросеть.

Желаете ли вы позволить **ARTIFICIAL INTELLIGENCE™** раскрыть тайну переменной X?

**Y — узнать истину**
**N — сохранить тайну**
)";

void InterractiveAiGrreting(){
	ColoredPrintf(MAGENTA, BLACK, _funnyAiGreeting);
}

void InterractiveAiGoodbye(){

}