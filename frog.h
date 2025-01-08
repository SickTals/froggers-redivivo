#ifndef FROG
#define FROG

#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

void frog(WINDOW **win, int pipefd[]);
msg handleFrog(pos p, msg f);
void printFrog(WINDOW **g_win, msg f);

#endif
