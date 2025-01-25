#ifndef FROG
#define FROG

#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define Y_STEP 2
#define X_STEP strlen(SPRITE_FROG)

void frog(WINDOW **win, int pipefd[]);
msg handleFrog(pos p, msg f);
void printFrog(WINDOW **g_win, msg f);
void granade(WINDOW **g_win, int pipefd[], int pipefd_projectiles[]);
void printGranade(WINDOW **g_win, msg g);

#endif
