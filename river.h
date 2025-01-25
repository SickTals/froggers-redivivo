#ifndef RIVER
#define RIVER

#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define NSPEEDS 3
#define COIN_FLIP rand() % 2
#define RAND_SPEEDS rand() % NSPEEDS
#define SPRITE_CROC '-'
#define SIZE_CROC 9
#define MIN_GAP 3
#define SPAWN_THRESHOLD MIN_GAP + SIZE_CROC
#define MOVE_STEP 1
#define INVALID_CROC -10
#define SPAWN_CHANCE 10

#define IS_VALID_LANE(lane) lane >= 0 && lane < NLANES
#define IS_ON_SCREEN(x) x >= 0 && x < GSIZE
#define SPAWN_TRY(free_lane, active_crocs) (free_lane >= SPAWN_THRESHOLD) && \
                                           (active_crocs < CROC_CAP) && \
                                           (rand() % SPAWN_CHANCE == 0)

typedef struct River {
  bool isRight; // 0 = left, 1 = right
  int speeds[NLANES];
} rvr;

rvr generateRiver();
pos invalidateCrocodile(pos c);
msg initCrocodiles(int speed);
bool isNotSpawned(int i, int n);
pos updateCrocodilePosition(pos croc, bool moveRight);
void river(rvr r, int speed, int pipefd[]);
void swapPos(pos a[], int i, int j);
msg handleCroc(pos p[], msg c);
msg* mergeCrocPos(pos s[], pos n[], pos f[]);
void printCrocodileType(WINDOW **g_win, pos crocs[]);
void printCrocs(WINDOW **g_win, pos slow[], pos normal[], pos fast[]);

#endif
