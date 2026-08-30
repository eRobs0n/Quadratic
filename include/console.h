#ifndef __CONSOLE__
#define __CONSOLE__

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

/**
 * Printf with custom background and text colors
 * @param [in] text_color color of the text
 * @param [in] bg_color color of the background
 * @param [in] fmt printf format string
 * @param [in] ... printf args
 * @note Only for MacOs zsh!
 */
void ColoredPrintf(enum ConsoleColors text_color, enum ConsoleColors bg_color, const char* fmt, ...);

//! Moves console caret to position (from top left)
void MoveCaret(unsigned string, unsigned column);

//! Printf with loading spinner for some time
void LoadingPrintf(int time, const char* fmt, ...);

//!Clear console and reset caret
void ResetConsole();

//!Disable echo in terminal
void DisableTerminalEcho();

//!Enable echo in terminal
void EnableTerminalEcho();


#endif /*__CONSOLE__*/