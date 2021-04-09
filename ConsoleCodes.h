#pragma once

// See: https://en.wikipedia.org/wiki/ANSI_escape_code#Colors

#define CONSOLE_HIDE_CURSOR		"\e[?25l"
#define CONSOLE_SHOW_CURSOR		"\e[?25h"
#define CONSOLE_CLEARSCREEN		"\e[2J\e[H"

#define CONSOLE_COLOR_DEFAULT	"\e[39;49m"
#define CONSOLE_DEFAULT			"\e[0m"

#define CONSOLE_TEXT_WHITE		"\e[37m"
#define CONSOLE_TEXT_BRIGHT		"\e[97m"
#define CONSOLE_TEXT_GRAY		"\e[90m"

#define CONSOLE_TEXT_DIMRED		"\e[31m"
#define CONSOLE_TEXT_DIMGREEN	"\e[32m"
#define CONSOLE_TEXT_DIMYELLOW	"\e[33m"
#define CONSOLE_TEXT_DIMBLUE	"\e[34m"
#define CONSOLE_TEXT_DIMMAGENTA	"\e[35m"
#define CONSOLE_TEXT_DIMCYAN	"\e[36m"

#define CONSOLE_TEXT_RED		"\e[91m"
#define CONSOLE_TEXT_GREEN		"\e[92m"
#define CONSOLE_TEXT_YELLOW		"\e[93m"
#define CONSOLE_TEXT_BLUE		"\e[94m"
#define CONSOLE_TEXT_MAGENTA	"\e[95m"
#define CONSOLE_TEXT_CYAN		"\e[96m"