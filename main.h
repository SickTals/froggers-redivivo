#ifndef MAIN
#define MAIN

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <curses.h>
#include <sys/wait.h>
#include "common.h"
#include "menu.h"
#include "frog.h"
#include "river.h"

#define NTASKS 5
#define SIZE_MSGS NTASKS + 1 - 2

void init_screen(WINDOW **g_win, WINDOW **ui_win);
void child_task(int i, WINDOW **g_win, int pipefd[], int pipefd_projectiles[], rvr r);
void end_screen(WINDOW **g_win, WINDOW **ui_win);
int game(WINDOW **g_win);

#endif
