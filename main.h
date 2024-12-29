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
#include "frog.h"
#include "common.h"
#include "menu.h"

#define NTASKS 1

void init_screen(WINDOW **g_win, WINDOW **ui_win, WINDOW **p_win);
void end_screen(WINDOW **g_win, WINDOW **ui_win, WINDOW **p_win);
int game(WINDOW **g_win);
msg manageFrog(int y, int x, msg f);
void printFrog(WINDOW **g_win, msg f);

#endif
