#include "console.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>

const char* const _CONSOLE_RESET = "\033[0m\033[K";
const int _COLOR_STR_MAX_LEN = 100;

const int _SPINNER_DELAY = 100000; /// <in microseconds
const char* const _SPINNER_LOAD = "/-\\|";

/**
 * Creates special string for console colored output
 * @param [out] res_color 
 * @param [in] res_color_len res_color string length
 * @param [in] text_color color of the text
 * @param [in] bg_color color of the background
 * @note Do not use!
 */
void _CreateColorStr(char* res_color, size_t res_color_len, enum ConsoleColors text_color, enum ConsoleColors bg_color);

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

	char res_color[_COLOR_STR_MAX_LEN];
	_CreateColorStr(res_color, _COLOR_STR_MAX_LEN, text_color, bg_color);
	printf("%s", res_color);
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("%s", _CONSOLE_RESET);
}

void MoveCaret(unsigned string, unsigned column){
	printf("\033[%d;%dH", string, column);
}

void LoadingPrintf(int time, const char* fmt, ...){
	time *= 1000000; //to microseconds

	int spinner_ind = 0;
	int spinner_load_str_len = strlen(_SPINNER_LOAD);
	while(time > 0){
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
		printf("\r [ %c ]", _SPINNER_LOAD[spinner_ind++ % spinner_load_str_len]);
		fflush(stdout);
        usleep(_SPINNER_DELAY);

        time -= _SPINNER_DELAY;
	}
}

void ResetConsole(){
	printf("\033[2J\033[H");
}

void DisableTerminalEcho(){
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void EnableTerminalEcho(){
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
