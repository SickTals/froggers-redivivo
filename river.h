#ifndef RIVER
#define RIVER

#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define NSPEEDS 3
#define UDELAY 500000

typedef struct River {
  bool firstDir; // 0 = sx, 1 = dx
  int speeds[NLANES];
} rvr;

rvr generateRiver();
void crocodile(rvr river, int speed, int pipefd[]);
void swapPos(pos a[], int i, int j);
msg handleCroc(pos p[], msg c);

#endif
