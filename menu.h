#ifndef MENU
#define MENU

#include <curses.h>
#include <string.h>
#include "common.h"

#define PLAY_MSG "PLAY"
#define OPTIONS_MSG "OPTIONS"
#define QUIT_MSG "QUIT"

gstate menu(WINDOW **g_win);
void printMenu(WINDOW **win, int cursor);
gstate manageMenu(WINDOW **win, int *cursor);

#endif
