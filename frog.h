#ifndef FROG
#define FROG

#include <ncurses.h>
#include <unistd.h>
#include "common.h"

void frog(WINDOW **win, int pipefd[]);

#endif
