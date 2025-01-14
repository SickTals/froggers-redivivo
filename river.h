#ifndef RIVER
#define RIVER

#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define NSPEEDS 3
#define UDELAY 500000
#define SPRITE_CROC '-'
#define SIZE_CROC 9
#define MIN_GAP 3
#define SPAWN_THRESHOLD MIN_GAP + SIZE_CROC
#define MOVE_STEP 1
#define INVALID_CROC -10

typedef struct River {
  bool firstDir; // 0 = sx, 1 = dx
  int speeds[NLANES];
} rvr;

rvr generateRiver();
void crocodile(rvr river, int speed, int pipefd[]);
void swapPos(pos a[], int i, int j);
msg handleCroc(pos p[], msg c);
void printCrocs(WINDOW **g_win, pos slow[], pos normal[], pos fast[]);

#endif
