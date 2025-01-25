#include "river.h"
#include "common.h"
#include "ncurses.h"


// Helper function to get message ID based on speed
static msgid getSpeedId(int speed)
{
    switch (speed) {
        case Slow:
            return Id_croc_slow;
        case Normal:
            return Id_croc_normal;
        case Fast:

            return Id_croc_fast;
        default:
            return Id_croc_normal;
    }
}

rvr generateRiver()
{
    rvr r;
    r.isRight = COIN_FLIP;
    for (int i = 0; i < NLANES; i++)
        r.speeds[i] = RAND_SPEEDS;
    return r;
}

static bool canSpawnCrocodile(pos crocs[], int count, int lane_y, bool moveRight, int spawn_x)
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

pos invalidateCrocodile(pos c)
{
    c.x = INVALID_CROC;
    c.y = INVALID_CROC;
    return c;
}

msg initCrocodiles(int speed)
{
    msg croc = {
        .id = getSpeedId(speed)
    };
    // Initialize all positions as invalid
    for (int i = 0; i < CROC_CAP; i++)
        croc.crocs [i] = invalidateCrocodile(croc.crocs[i]);

    return croc;
}

pos updateCrocodilePosition(pos croc, bool moveRight)
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

pos updateCrocodile(pos c, bool isRight, int *active_crocs)
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

pos compactCrocs(pos c[], int i, int *validIdx)
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

pos spawnCrocodile(pos c[], int lane, bool isRight, int validIdx)
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

void river(rvr r, int speed, int pipefd[])
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
                usleep(UDELAY/2);
                break;
        }
    }
}

msg handleCroc(pos p[], msg c) {
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
// Helper function to print a single crocodile type
void printCrocodileType(WINDOW **g_win, pos crocs[])
{
    for (int j = 0; j < CROC_CAP; j++) {
        if (crocs[j].x == INVALID_CROC || crocs[j].y == INVALID_CROC)
            break;
        for (int i = 0; i < SIZE_CROC; i++) {
            if (!(IS_ON_SCREEN(crocs[j].x + i)))
                continue;
            mvwaddch(*g_win, crocs[j].y, crocs[j].x + i, SPRITE_CROC);
            mvwaddch(*g_win, crocs[j].y - 1, crocs[j].x + i, SPRITE_CROC);
        }
    }
}

void printCrocs(WINDOW **g_win, pos slow[], pos normal[], pos fast[])
{
    printCrocodileType(g_win, slow);
    printCrocodileType(g_win, normal);
    printCrocodileType(g_win, fast);
}
