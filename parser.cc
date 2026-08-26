#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "solver.h"
#include "common.h"

enum TermCoefficient _ProcessTerm(const char* t, double* coeff){
	#ifdef _DEBUG
		debug_printf("_ProcessTerm: runned with term %s\n", t);
	#endif

	assert(t != NULL);
	assert(coeff != NULL);

	double t_coeff = 0.;

	char var[MAX_TERM_LENGTH] = {0};

	int parseCnt = sscanf(t, "%lf%s", &t_coeff, var);

	#ifdef _DEBUG
		debug_printf("_ProcessTerm: parsed %d values %s\n", parseCnt, var);
	#endif

	if (parseCnt == 0){
		#ifdef _DEBUG
		debug_printf("_ProcessTerm: Isn't full term\n");
		#endif

		parseCnt = sscanf(t, "%s", var);
		t_coeff = 1;
	}

	if (strlen(var) == 0){
		*coeff = t_coeff;
		return CONSTANT_TERM;
	}

	bool is_negative = false;
	if (var[0] == '-') is_negative = true;
	StrSqueeze(var, '-');
	StrSqueeze(var, '+');

	for (unsigned i = 0; i < ARR_LEN(_LEADING_VAR_STRINGS); i++){
		if (strcmp(var, _LEADING_VAR_STRINGS[i]) == 0){
			*coeff = (is_negative ? -t_coeff : t_coeff);
			return LEADING_TERM;
		}
	}

	for (unsigned i = 0; i < ARR_LEN(_LINEAR_VAR_STRINGS); i++){
		if (strcmp(var, _LINEAR_VAR_STRINGS[i]) == 0){
			*coeff = (is_negative ? -t_coeff : t_coeff);
			return LINEAR_TERM;
		}
	}

	return INVALID_TERM;
}

//!In the expression string moves terms to the left (by changing sign)
//!In case of error not changes string
void MoveTermsLeftInStr(char* expr){ //...=-2
	assert(expr != NULL);

	int left_p_size = 0;
	while(*expr != '=') {
		expr++;
		left_p_size++;
		if (*expr == '\0') return;
	}
	if(left_p_size == 0) return;

	if (*(expr+1) != '\0'){
		if (*(expr+1) == '-'){
			*(expr + 1) = '+';
			*expr++ = ' ';
		}else if (*(expr+1) == '+'){
			*(expr + 1) = '-';
			*expr++ = ' ';
		}else{
			*expr++ = '-';
		}
	}
	while(*expr != '\0'){
		if (*expr == '+') *expr = '-';
		else if (*expr == '-') *expr = '+';
		expr++;
	}
}

enum ParsingStatus ParseExpression(const char* expr, struct EquationCoeffs* coeff){
	assert(expr != NULL);

	char expr_cpy[MAX_EXPRESSION_LENGTH] = "";

	strcpy(expr_cpy, expr);

	StrSqueeze(expr_cpy, ' ');
	StrSqueeze(expr_cpy, '\t');

	MoveTermsLeftInStr(expr_cpy);
	//StrSqueeze(expr_cpy, ' ');

	#ifdef _DEBUG
		debug_printf("ParseExpression: After moving terms to the left %s\n", expr_cpy);
	#endif

	const  char* token_ptr = expr_cpy;
	char term[MAX_TERM_LENGTH] = {0};
	
	struct EquationCoeffs calc_coeffs = 
		{.coeff_of_sq_x = 0., .coeff_of_x = 0., .free_coeff = 0};

	while ((token_ptr = SepByTerms(token_ptr, term, MAX_TERM_LENGTH)) != NULL){
		#ifdef _DEBUG
			//debug_printf("ParseExpression: separated term %s\n", term);
		#endif

		double pred_coeff = 0.;
		enum TermCoefficient type = _ProcessTerm(term, &pred_coeff);

		#ifdef _DEBUG
			//debug_printf("ParseExpression: extracted coefficeint %lf\n", pred_coeff);
		#endif

		#ifdef _DEBUG
			//debug_printf("ParseExpression: returned status: %d\n", type);
		#endif

		switch(type){
		case INVALID_TERM:
			return PARSING_ERROR;
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
		case TERM_COEFFICIENT_MIN_VAL:
			assert(false && "Invalid enum in ParseExpression");
			break;
		case TERM_COEFFICIENT_MAX_VAL:
			assert(false && "Invalid enum in ParseExpression");
			break;
		default:
			assert(false && "Invalid enum in ParseExpression");
			break;
		}
		memset(term, 0, MAX_TERM_LENGTH*sizeof(char));
	}
	*coeff = calc_coeffs;
	return PARSING_OK;
}

const char* SepByTerms(const char* str, char* term, size_t max_term_length){
   if (*str == '\0') return NULL;
   char c;
   size_t pos = 0;
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
 
void StrSqueeze(char s[], int c){
   int i, j;
   for(i = j = 0; s[i] != '\0'; i++){
      if (s[i] != c)
         s[j++] = s[i];
   }
   s[j] = '\0';
}

