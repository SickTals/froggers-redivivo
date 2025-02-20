#ifndef FROG
#define FROG

#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"

#define WIDTH_FROG 3


enum Grenades {
    Idx_sx_grenade = 0,
    Idx_dx_grenade = 1,
    N_grenades = 2
};

void frog(WINDOW **win, int pipefd[]);
msg handleFrog(obj incoming, obj frog);
void printFrog(WINDOW **win, obj frog);
void granade(int pipefd[], int pipefd_grenade[]);
void printGranade(WINDOW **win, obj grenade[N_grenades]);

#endif
