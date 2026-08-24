#ifndef __PARSER__
#define __PARSER__
#include <stddef.h>

const int MAX_EXPRESSION_LENGTH = 200;
const int MAX_TERM_LENGTH = 100;

enum TermCoefficient{
	INVALID_TERM  = -1,
	CONSTANT_TERM =  0,
	LINEAR_TERM   =  1,
	LEADING_TERM  =  2
};

enum ParsingStatus{
	PARSING_ERROR,
	PARSING_OK
};

//! Remove all c characters in s[] string
void StrSqueeze(char s[], int c);

/**
 * Process one term of math expression. Extracts coefficient
 * @param [in] t term string
 * @param [out] coeff Pointer to coefficients
 * @return TermCoefficeint (invalid, const, linear or leading)
 * @note Do not use!
 */
enum TermCoefficient _ProcessTerm(const char* t, double* coeff);

/**
 * Separate string by terms (by + or - symbol)
 * @param [in] str String for separating
 * @param [out] term Pointer to term string
 * @param [in] max_term_length maximum length of one term
 * @return A pointer to the beginning of the next term in the string
 * @note In case of error or if the term longer then max_term_length returns NULL!
 */
const char* SepByTerms(const char* str, char* term, size_t max_term_length);

/**
 * Parse math expression
 * @param [in] expr Expression string
 * @param [out] coeff Pointer to term string
 * @return ParsingStatus (ERROR or OK)
 */
enum ParsingStatus ParseExpression(const char* expr, struct EquationCoeffs* coeff);

#endif /* __PARSER__ */