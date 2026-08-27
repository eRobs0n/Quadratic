#include "ai.h"
#include "console.h"
#include "io.h"
#include "solver.h"
#include "stdio.h"
#include "ctype.h"

void RequestCard(){
	ResetConsole();
	char card[32];
	printf(_cardForm);
	fflush(stdout);

	MoveCaret(_cardFromNumber, _cardFormColumn);
	fflush(stdout);

	DisableTerminalEcho();
	char c = 0;
	int cnt_nm = 0;
	while((c = getchar()) != '\n' && c != EOF && cnt_nm < 16){
		if (isdigit(c)){
			cnt_nm+=1;
			EnableTerminalEcho();
			putchar(c);
			if(cnt_nm%4==0) putchar(' ');
			DisableTerminalEcho();
		}
	}
	EnableTerminalEcho();

	DisableTerminalEcho();
    MoveCaret(_cardFromExp, _cardFormColumn);
	c = 0; cnt_nm = 0;
	while((c = getchar()) != '\n' && c != EOF && cnt_nm <4){
		if (isdigit(c)){
			cnt_nm+=1;
			EnableTerminalEcho();
			putchar(c);
			if(cnt_nm == 2) putchar('/');
			DisableTerminalEcho();
		}
	}

    MoveCaret(_cardFromCVV, _cardFormColumn);
	
	c = 0; cnt_nm = 0;
	while((c = getchar()) != '\n' && c != EOF && cnt_nm <3){
		if (isdigit(c)){
			cnt_nm+=1;
			EnableTerminalEcho();
			putchar(c);
			DisableTerminalEcho();
		}
	}
    ResetConsole();

	EnableTerminalEcho();
}

void AiMode(){
	#ifndef _DEBUG
		ResetConsole();
	#endif
	ColoredPrintf(MAGENTA, BLACK, _funnyAiGreeting);
	enum YesNoInputStatus inp = YesNoInput();

	LoadingPrintf(2, "Connecting...");

	ClearStdinBuffer();

	ResetConsole();
	if (inp == NO_ANS){
		ColoredPrintf(MAGENTA, BLACK, _funnyAiInvalid);
		LoadingPrintf(2, "Loading...");
	}

	ResetConsole();
	ColoredPrintf(RED, NO_COLOR, _aiOutOfTokens);
	inp = YesNoInput();
	ClearStdinBuffer();

	RequestCard();

	LoadingPrintf(2, "Processing payment...");
	ColoredPrintf(GREEN, NO_COLOR, _aiPaymentProceed);
	getchar();

	ResetConsole();
	struct EquationCoeffs coeffs = {};
	RequestExpression(&coeffs, _aiRequestExpression);
	struct EquationSolutions roots;
	SolveEquation(&coeffs, &roots);

	LoadingPrintf(3, "Thinking...");
	ResetConsole();

	switch(roots.nRoots){
	case NO_ROOTS:
		ColoredPrintf(MAGENTA, BLACK, "✅ Отличное уравнение! Но корней у него нет");
		break;
	case ONE_ROOT:
		ColoredPrintf(MAGENTA, BLACK, "✅ Я нашел только корень %lf", roots.root1);
		break;
	case TWO_ROOTS:
		ColoredPrintf(MAGENTA, BLACK, "✅ Корни уравенния %lf и %lf", roots.root1, roots.root2);
		break;
	case INF_ROOTS:
		ColoredPrintf(MAGENTA, BLACK, "✅ У этого уравнения слишком много корней");
		break;
	}
	printf("\n");
	//PrintRoots(&roots, DEFAULT_ACCURACY);
}