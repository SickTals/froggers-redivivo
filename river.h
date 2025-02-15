#ifndef RIVER
#define RIVER

#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "common.h"

#define NSPEEDS 3
#define COIN_FLIP rand() % 2
#define RAND_SPEEDS (2 + (rand() % NSPEEDS))
#define SPRITE_CROC '-'
#define SIZE_CROC 9
#define MIN_GAP 3
#define SPAWN_THRESHOLD MIN_GAP + SIZE_CROC
#define MOVE_STEP 1
#define INVALID_CROC -10
#define SPAWN_CHANCE 10
#define SHOOT_CHANCE !(rand() % 25)

#define IS_VALID_LANE(lane) lane >= 0 && lane < NLANES
#define IS_ON_SCREEN(x) x >= 0 && x < GSIZE
#define SPAWN_TRY(free_lane, active_crocs) (free_lane >= SPAWN_THRESHOLD) && \
                                           (active_crocs < CROC_CAP) && \
                                           (rand() % SPAWN_CHANCE == 0)

typedef struct River {
  bool isRight; // 0 = left, 1 = right
  enum Speeds speeds[NLANES];
} rvr;

rvr generateRiver();
obj invalidateObject();
msg initCrocodiles(enum Speeds speed);
obj updateCrocodilePosition(obj croc, bool moveRight);
void river(rvr r, enum Speeds speed, int pipefd[]);
msg handleCroc(obj p[], msg c);
void printCrocs(WINDOW **g_win, msg *c, int nspeeds, bool isRight);
void projectile(int pipefd[], int pipefd_projectiles[], bool isRight);
void printCrocProjectile(WINDOW **g_win, msg p);

#endif
