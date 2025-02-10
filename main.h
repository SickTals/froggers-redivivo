#ifndef MAIN
#define MAIN

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <curses.h>
#include <errno.h>
#include <sys/wait.h>
#include "common.h"
#include "menu.h"
#include "frog.h"
#include "river.h"

#define NTASKS 6
#define NDENS 5

void init_screen(WINDOW **g_win, WINDOW **ui_win);
void child_task(int i, WINDOW **g_win, int pipefd[], int pipefd_projectiles[], int pipefd_grenade[], rvr r);
void end_screen(WINDOW **g_win, WINDOW **ui_win);
bool isDrawning(pos f, msg *c, int nspeeds);
bool isShot (int proj_active, pos f, msg proj);
bool den(bool dens[5], pos frog_pos);
void printDens(WINDOW **win, bool dens[NDENS]);
gstate collisions(msg msgs[], bool dens[NDENS], int proj_active);
gstate game(WINDOW **g_win, bool dens[NDENS]);

#endif
