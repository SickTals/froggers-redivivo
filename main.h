#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <curses.h>
#include <sys/wait.h>
#include "frog.h"
#include "common.h"

#define GSIZE 56 //36 fiume, 2*10 margini, 16x16 * 3.5
#define PSIZE 56/2
#define UISIZE 8
#define PLAY_MSG "PLAY"
#define OPTIONS_MSG "OPTIONS"
#define QUIT_MSG "QUIT"
#define NTASKS 1
#define SPRITE_CURSOR '>'
#define SPRITE_FROG ">M<"

void init_screen(WINDOW **g_win, WINDOW **ui_win, WINDOW **p_win);
void end_screen(WINDOW **g_win, WINDOW **ui_win, WINDOW **p_win);
bool menu(WINDOW **g_win, int flag);
int game(WINDOW **g_win);
msg manageFrog(int y, int x, msg f);
void printFrog(WINDOW **g_win, msg f);

#endif
