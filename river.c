#include "river.h"
#include "common.h"
#include "ncurses.h"

rvr generateRiver()
{
    rvr r;
    r.isRight = COIN_FLIP;
    for (int i = 0; i < NLANES; i++)
        r.speeds[i] = RAND_SPEEDS;
    return r;
}

static bool canSpawnCrocodile(obj crocs[], int count, int lane_y, bool moveRight, int spawn_x)
{
    for (int i = 0; i < count; i++) {
        if (crocs[i].y != lane_y)
            continue;
        int gap = moveRight ? 
            abs(spawn_x - crocs[i].x) :
            abs((spawn_x + SIZE_CROC) - (crocs[i].x + SIZE_CROC));
        if (gap < MIN_GAP)
            return false;
    }
    return true;
}

obj invalidateCrocodile(obj c)
{
    c.x = INVALID_CROC;
    c.y = INVALID_CROC;
    return c;
}

msg initCrocodiles(enum Speeds speed)
{
    msg croc = {
        .id = (msgid)speed
    };
    // Initialize all positions as invalid
    for (int i = 0; i < CROC_CAP; i++)
        croc.crocs [i] = invalidateCrocodile(croc.crocs[i]);

    return croc;
}

obj updateCrocodilePosition(obj croc, bool moveRight)
{
    if (moveRight) {
        croc.x += MOVE_STEP;
        if (croc.x >= GSIZE)
            croc = invalidateCrocodile(croc);
    } else {
        croc.x -= MOVE_STEP;
        if (croc.x < -(SIZE_CROC + 1))
            croc = invalidateCrocodile(croc);
    }
    return croc;
}

obj updateCrocodile(obj c, bool isRight, int *active_crocs)
{
    if (c.x == INVALID_CROC || c.y == INVALID_CROC)
        return c;

    int lane = (GSIZE/2 - 2 - c.y - 1) / 2;
    
    if (!(IS_VALID_LANE(lane))) {
        c = invalidateCrocodile(c);
        return c;
    }
    
    bool moveRight = (isRight + lane) % 2 == 0;
    c = updateCrocodilePosition(c, moveRight);
    
    if (c.x != INVALID_CROC)
        *active_crocs += 1;

    return c;
}

obj compactCrocs(obj c[], int i, int *validIdx)
{
    if (c[i].x == INVALID_CROC && c[i].y == INVALID_CROC)
        return c[i];
    if (i != *validIdx) {
        c[*validIdx] = c[i];
        c[i] = invalidateCrocodile(c[i]);
    }
    *validIdx += 1;
    return c[i];
}

obj spawnCrocodile(obj c[], int lane, bool isRight, int validIdx)
{
    int lane_y = GSIZE/2 - 2 - (1 + lane) * 2;
    bool moveRight = (isRight + lane) % 2 == 0;
    int spawn_x = moveRight ? 0 : GSIZE - SIZE_CROC;
    
    if (!canSpawnCrocodile(c, validIdx, lane_y, moveRight, spawn_x))
        return c[validIdx];
    c[validIdx].y = lane_y;
    c[validIdx].x = spawn_x;
    return c[validIdx];

}

void river(rvr r, enum Speeds speed, int pipefd[])
{
    close(pipefd[0]);
    
    msg croc = initCrocodiles(speed);
    
    int free_counter[NLANES] = {0};
    int active_crocs = 0;
    
    while (true) {
        active_crocs = 0;
        
        // Update existing crocodiles
        for (int i = 0; i < CROC_CAP; i++)
            croc.crocs[i] = updateCrocodile(croc.crocs[i], r.isRight, &active_crocs);
        
        // Compact array
        int validIdx = 0;
        for (int i = 0; i < CROC_CAP; i++)
            croc.crocs[i] = compactCrocs(croc.crocs, i, &validIdx);

        // projectiles
        for (int i = 0; i < CROC_CAP; i++) {
            if (croc.crocs[i].x == INVALID_CROC || croc.crocs[i].y == INVALID_CROC)
                break;
            croc.crocs[i].shoots = SHOOT_CHANCE;
        }
        
        // Spawn new crocodiles
        for (int lane = 0; lane < NLANES; lane++) {
            if (r.speeds[lane] != speed)
                continue;
            free_counter[lane]++;
            if (!(SPAWN_TRY(free_counter[lane], active_crocs)))
                continue;
            croc.crocs[validIdx] = spawnCrocodile(croc.crocs, lane, r.isRight, validIdx);
            active_crocs++;
            free_counter[lane] = 0;
        }
        
        write(pipefd[1], &croc, sizeof(croc));
        
        // Sleep based on speed
        switch (speed) {
            case Slow: 
                usleep(UDELAY * 2);
                break;
            case Normal:
                usleep(UDELAY);
                break;
            case Fast:
                usleep(UDELAY / 2);
                break;
        }
    }
}

msg handleCroc(obj p[], msg c) {
    // Clear all positions
    for (int i = 0; i < CROC_CAP; i++)
        c.crocs[i] = invalidateCrocodile(c.crocs[i]);
    
    // Copy valid positions
    int validIdx = 0;
    for (int i = 0; i < CROC_CAP; i++) {
        if (p[i].x == INVALID_CROC || p[i].y == INVALID_CROC)
            continue;
        c.crocs[validIdx] = p[i];
        validIdx++;
    }
    return c;
}

void printCrocs(WINDOW **g_win, msg *c, int nspeeds)
{
    for (int s = 0; s < nspeeds; s++)
    {
        for (int j = 0; j < CROC_CAP; j++) {
            if (c[s].crocs[j].x == INVALID_CROC || c[s].crocs[j].y == INVALID_CROC)
                break;
            for (int i = 0; i < SIZE_CROC; i++) {
                if (!(IS_ON_SCREEN(c[s].crocs[j].x + i)))
                    continue;
                mvwaddch(*g_win, c[s].crocs[j].y, c[s].crocs[j].x + i, SPRITE_CROC);
                mvwaddch(*g_win, c[s].crocs[j].y - 1, c[s].crocs[j].x + i, SPRITE_CROC);
            }
        }
        
    }
}
