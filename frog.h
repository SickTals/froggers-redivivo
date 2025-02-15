#ifndef FROG
#define FROG

#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"

#define Y_STEP 2
#define X_STEP strlen(SPRITE_FROG)

void frog(WINDOW **win, int pipefd[]);
msg handleFrog(obj incoming, obj frog);
void printFrog(WINDOW **g_win, obj frog);
void granade(int pipefd[], int pipefd_grenade[]);
void printGranade(WINDOW **g_win, msg g);

#endif
