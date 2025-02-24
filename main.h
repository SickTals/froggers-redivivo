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
#include <pthread.h>
#include "common.h"
#include "menu.h"
#include "ui.h"
#include "frog.h"
#include "river.h"
#include "dens.h"

#define WIN_START_Y ((LINES - GSIZE / 2) / 2)
#define WIN_START_X (((COLS - GSIZE) / 2) - UISIZE / 2)
#define PWIN_START_Y ((LINES - PSIZE / 3) / 2)
#define PWIN_START_X (((COLS - PSIZE) - UISIZE) / 2)

#define CENTER_X (GSIZE / 2)
#define SIDEWALK_Y (GSIZE/2 - 2)
#define BOX_BORDER 1

#define IS_CROC_SHOOTING msgs[NTASKS].objs[i].x != INVALID_CROC && \
                        msgs[NTASKS].objs[i].y != INVALID_CROC && \
                        msgs[NTASKS].objs[i].shoots && \
                        *croc_projectiles_active < CROC_CAP

#define NLIVES 3
#define DIM_BUFFER 64

typedef struct SharedData{
    msg msgs[NTASKS + 1]; // Stores messages for all tasks
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SharedData;

void init_screen(WINDOW **g_win, WINDOW **ui_win);
void end_screen(WINDOW **win, bool dens[], int h, const char *sprite[], int alt_color);
void initObjects(msg msgs[]);
void child_task(int i, WINDOW **g_win, int pipefd[], int pipefd_projectiles[], int pipefd_grenade[], rvr r);
void kill_screen(WINDOW **g_win, WINDOW **ui_win, bool dens[NDENS]);
bool isDrawning(obj f, msg *c, int nspeeds);
bool isShot(int proj_active, obj f, msg proj);
gstate hasWon(bool dens[NDENS]);
void init_bckg(WINDOW **win);
gstate collisions(msg msgs[], bool dens[NDENS], bool isRight, int pipefd_projectiles[], int pipefd_grenade[], int proj_active);
bool sendGrenadeShot(int pipefd[], msg f);
int sendProjectileShot(int pipefd[], obj c, int n);
int updateProjectileCount(obj p[]);
msg handleObject(msg msgs[NTASKS + 1], int pipefd_grenade[], int pipefd_projectiles[], bool *grenade_active, int *croc_projectiles_active, bool isRight);
gstate game(WINDOW **g_win, WINDOW **ui_win, int lives, int score, bool dens[NDENS]);

#endif
