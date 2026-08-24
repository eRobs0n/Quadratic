#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "solver.h"
#include "common.h"

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

enum TermCoefficient _ProcessTerm(const char* t, double* coeff){
	#ifdef _DEBUG
	printf("Runned _ProcessTerm with term %s\n", t);
	#endif

	assert(t != NULL);
	assert(coeff != NULL);

	double t_coeff = 0.;

	char var[MAX_TERM_LENGTH];

	bool full_term = true;
	int parseCnt = sscanf(t, "%lf%s", &t_coeff, var);

	#ifdef _DEBUG
	printf("Parsed %d values %s\n", parseCnt, var);
	#endif

	if (parseCnt == 0){
		#ifdef _DEBUG
		printf("Isn't full term\n");
		#endif

		full_term = false;
		parseCnt = sscanf(t, "%s", var);
		t_coeff = 1;
	}

	if (strlen(var) == 0){
		*coeff = t_coeff;
		return CONSTANT_TERM;
	}

	bool is_negative = false;
	if (var[0] == '-') is_negative = true;

	#ifdef _DEBUG
	printf("Negative %d\n", is_negative);
	#endif

	StrSqueeze(var, '-');
	StrSqueeze(var, '+');

	#ifdef _DEBUG
		printf("before p strings coeff is %c%lf\n", (is_negative) ? '+' : '-', t_coeff);
	#endif

	for (int i = 0; i < ARR_LEN(_LEADING_VAR_STRINGS); i++){
		if (strcmp(var, _LEADING_VAR_STRINGS[i]) == 0){
			*coeff = (is_negative ? -t_coeff : t_coeff);
			return LEADING_TERM;
		}
	}

	for (int i = 0; i < ARR_LEN(_LINEAR_VAR_STRINGS); i++){
		if (strcmp(var, _LINEAR_VAR_STRINGS[i]) == 0){
			*coeff = (is_negative ? -t_coeff : t_coeff);
			return LINEAR_TERM;
		}
	}

	return INVALID_TERM;
}

enum ParsingStatus ParseExpression(const char* expr, struct EquationCoeffs* coeff){
	assert(expr != NULL);

	const  char* token_ptr = expr;
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

		#ifdef _DEBUG
			printf("Returned status: %d\n", type);
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
		default:
			assert(false);
		}

		memset(term, 0, MAX_TERM_LENGTH*sizeof(char));
	}
	*coeff = calc_coeffs;
	return PARSING_OK;
}

const char* SepByTerms(const char* str, char* term, size_t max_term_length){
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

