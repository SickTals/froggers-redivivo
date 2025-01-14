#include "river.h"
#include "common.h"
#include "ncurses.h"

rvr generateRiver()
{
    rvr r;
    r.firstDir = rand() % 2;
    for(int i = 0; i < NLANES; i++) {
        r.speeds[i] = rand() % NSPEEDS;
    }
    return r;
}

void crocodile(rvr river, int speed, int pipefd[]) {
    close(pipefd[0]);
    int free_counter[NLANES];
    int ncrocs = 0;
    msg msg_croc;

    switch (speed) {
        case Slow:
            msg_croc.id = Id_croc_slow;
            break;
        case Normal:
            msg_croc.id = Id_croc_normal;
            break;
        case Fast: 
            msg_croc.id = Id_croc_fast;
            break;
    }

    for (int i = 0; i < NLANES*8; i++) {
        msg_croc.crocs[i].y = INVALID_CROC;
        msg_croc.crocs[i].x = INVALID_CROC;
    }
    for (int i = 0; i < NLANES; i++) {
        free_counter[i] = SPAWN_THRESHOLD;
    }
    
    while (true) {
        ncrocs = 0;
        
        for (int i = 0; i < NLANES*8; i++) {
            if (msg_croc.crocs[i].x != INVALID_CROC && msg_croc.crocs[i].y != INVALID_CROC) {
                int lane = (GSIZE/2 - 2 - msg_croc.crocs[i].y - 1) / 2;
                if (lane < 0 || lane >= NLANES) {
                    msg_croc.crocs[i].x = INVALID_CROC;
                    msg_croc.crocs[i].y = INVALID_CROC;
                    continue;
                }
                
                bool moveRight = (river.firstDir + lane) % 2 == 0;
                
                if (moveRight) {
                    msg_croc.crocs[i].x += MOVE_STEP;
                    if (msg_croc.crocs[i].x >= GSIZE) {
                        msg_croc.crocs[i].x = INVALID_CROC;
                        msg_croc.crocs[i].y = INVALID_CROC;
                        continue;
                    }
                } else {
                    msg_croc.crocs[i].x -= MOVE_STEP;
                    if (msg_croc.crocs[i].x < -(SIZE_CROC + 1)) {
                        msg_croc.crocs[i].x = INVALID_CROC;
                        msg_croc.crocs[i].y = INVALID_CROC;
                        continue;
                    }
                }
                ncrocs++;
            }
        }

        int validIdx = 0;
        for (int i = 0; i < NLANES*8; i++) {
            if (msg_croc.crocs[i].x != INVALID_CROC && msg_croc.crocs[i].y != INVALID_CROC) {
                if (i != validIdx) {
                    msg_croc.crocs[validIdx] = msg_croc.crocs[i];
                    msg_croc.crocs[i].x = INVALID_CROC;
                    msg_croc.crocs[i].y = INVALID_CROC;
                }
                validIdx++;
            }
        }

        for (int i = validIdx; i < NLANES*8; i++) {
            msg_croc.crocs[i].x = INVALID_CROC;
            msg_croc.crocs[i].y = INVALID_CROC;
        }

        for (int lane = 0; lane < NLANES; lane++) {
            if (river.speeds[lane] != speed) {
                continue;
            }
            
            free_counter[lane]++;
            
            if (free_counter[lane] >= SPAWN_THRESHOLD && ncrocs < NLANES*8 && rand() % 10 == 0) {
                int lane_y = GSIZE/2 - 2 - (1+lane)*2;
                bool moveRight = (river.firstDir + lane) % 2 == 0;
                int spawn_x = moveRight ? 0 : GSIZE - SIZE_CROC;
                
                bool canSpawn = true;
                for (int i = 0; i < validIdx; i++) {
                    if (msg_croc.crocs[i].y == lane_y && msg_croc.crocs[i].x != INVALID_CROC) {
                        int gap = moveRight ? 
                            abs(spawn_x - msg_croc.crocs[i].x) :
                            abs((spawn_x + SIZE_CROC) - (msg_croc.crocs[i].x + SIZE_CROC));
                        if (gap < MIN_GAP) {
                            canSpawn = false;
                            break;
                        }
                    }
                }
                
                if (canSpawn) {
                    int nextIdx = 0;
                    while (nextIdx < NLANES*8 && 
                           msg_croc.crocs[nextIdx].x != INVALID_CROC && 
                           msg_croc.crocs[nextIdx].y != INVALID_CROC) {
                        nextIdx++;
                    }
                    if (nextIdx < NLANES*8) {
                        msg_croc.crocs[nextIdx].y = lane_y;
                        msg_croc.crocs[nextIdx].x = spawn_x;
                        ncrocs++;
                        free_counter[lane] = 0;
                    }
                }
            }
        }

        write(pipefd[1], &msg_croc, sizeof(msg_croc));

        switch (speed) {
            case Slow: usleep(UDELAY*2); break;
            case Normal: usleep(UDELAY); break;
            case Fast: usleep(UDELAY/2); break;
        }
    }
    close(pipefd[1]);
}
msg handleCroc(pos p[], msg c) {
    for (int i = 0; i < NLANES*8; i++) {
        c.crocs[i].x = INVALID_CROC;
        c.crocs[i].y = INVALID_CROC;
    }
    
    int validIdx = 0;
    for (int i = 0; i < NLANES*8; i++) {
        if (p[i].x != INVALID_CROC && p[i].y != INVALID_CROC) {
            c.crocs[validIdx] = p[i];
            validIdx++;
        }
    }
    return c;
}

void printCrocs(WINDOW **g_win, pos slow[], pos normal[], pos fast[])
{
    for (int j = 0; j < NLANES*8; j++) {
        if (slow[j].x == INVALID_CROC || slow[j].y == INVALID_CROC)
            break;
        for (int i = 0; i < SIZE_CROC; i++) {
            if (slow[j].x + i >= 0 && slow[j].x + i < GSIZE) {
                mvwaddch(*g_win, slow[j].y, slow[j].x + i, SPRITE_CROC);
                mvwaddch(*g_win, slow[j].y - 1, slow[j].x + i, SPRITE_CROC);
            }
        }
    }
    
    for (int j = 0; j < NLANES*8; j++) {
        if (normal[j].x == INVALID_CROC || normal[j].y == INVALID_CROC)
            break;
        for (int i = 0; i < SIZE_CROC; i++) {
            if (normal[j].x + i >= 0 && normal[j].x + i < GSIZE) {
                mvwaddch(*g_win, normal[j].y, normal[j].x + i, SPRITE_CROC);
                mvwaddch(*g_win, normal[j].y - 1, normal[j].x + i, SPRITE_CROC);
            }
        }
    }
    
    for (int j = 0; j < NLANES*8; j++) {
        if (fast[j].x == INVALID_CROC || fast[j].y == INVALID_CROC)
            break;
        for (int i = 0; i < SIZE_CROC; i++) {
            if (fast[j].x + i >= 0 && fast[j].x + i < GSIZE) {
                mvwaddch(*g_win, fast[j].y, fast[j].x + i, SPRITE_CROC);
                mvwaddch(*g_win, fast[j].y - 1, fast[j].x + i, SPRITE_CROC);
            }
        }
    }
}
