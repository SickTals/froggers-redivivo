#ifndef UI
#define UI

#include <ncurses.h>
#include <unistd.h>
#include "common.h"

#define TIME 60
#define SIZE_FSCORE 16
#define START_X 2
#define TIMER_Y 3
#define CENTER_X 3
#define LIVES_Y 6
#define SCORE_Y 11

void printMenuUi(WINDOW **win);
void timer(int pipefd[]);
void printUi(WINDOW **win, msg t, int lives, int score);

#endif
