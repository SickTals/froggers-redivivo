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
        croc.objs [i] = invalidateCrocodile(croc.objs[i]);

    return croc;
}

obj updateCrocodilePosition(obj croc, bool moveRight)
{
    croc.shoots = SHOOT_CHANCE;

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
    int active_crocs;
    
    while (true) {
        active_crocs = 0;
        
        // Update existing crocodiles
        for (int i = 0; i < CROC_CAP; i++)
            croc.objs[i] = updateCrocodile(croc.objs[i], r.isRight, &active_crocs);
        
        // Compact array
        int validIdx = 0;
        for (int i = 0; i < CROC_CAP; i++)
            croc.objs[i] = compactCrocs(croc.objs, i, &validIdx);

        // Spawn new crocodiles
        for (int lane = 0; lane < NLANES; lane++) {
            if (r.speeds[lane] != speed)
                continue;
            free_counter[lane]++;
            if (!(SPAWN_TRY(free_counter[lane], active_crocs)))
                continue;
            croc.objs[validIdx] = spawnCrocodile(croc.objs, lane, r.isRight, validIdx);
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
        c.objs[i] = invalidateCrocodile(c.objs[i]);

    // Copy valid positions
    int validIdx = 0;
    for (int i = 0; i < CROC_CAP; i++) {
        if (p[i].x == INVALID_CROC || p[i].y == INVALID_CROC)
            continue;
        c.objs[validIdx] = p[i];
        validIdx++;
    }
    
    if (c.shoots)
        c.id = Id_croc_projectile;

    return c;
}

void printCrocs(WINDOW **g_win, msg *c, int nspeeds)
{
    for (int s = 0; s < nspeeds; s++)
    {
        for (int j = 0; j < CROC_CAP; j++) {
            if (c[s].objs[j].x == INVALID_CROC || c[s].objs[j].y == INVALID_CROC)
                break;
            for (int i = 0; i < SIZE_CROC; i++) {
                if (!(IS_ON_SCREEN(c[s].objs[j].x + i)))
                    continue;
                mvwaddch(*g_win, c[s].objs[j].y, c[s].objs[j].x + i, SPRITE_CROC);
                mvwaddch(*g_win, c[s].objs[j].y - 1, c[s].objs[j].x + i, SPRITE_CROC);
            }
        }
        
    }
}


void projectile(int pipefd[], int pipefd_projectiles[], bool isRight) {
    msg projectiles = {
        .id = Id_croc_projectile
    };
    
    // Initialize all projectiles as invalid
    for (int i = 0; i < CROC_CAP; i++)
        projectiles.objs[i] = invalidateCrocodile(projectiles.objs[i]);

    // Set non-blocking read
    int flags = fcntl(pipefd_projectiles[0], F_GETFL, 0);
    fcntl(pipefd_projectiles[0], F_SETFL, flags | O_NONBLOCK);
    
    close(pipefd[0]);
    
    while (true) {
        // Handle new shooting crocodiles
        msg tmp;
        ssize_t n = read(pipefd_projectiles[0], &tmp, sizeof(msg));
        if (n == sizeof(msg) && tmp.shoots) {
            for (int i = 0; i < CROC_CAP; i++) {
                if (!tmp.objs[i].shoots && tmp.objs[i].x == INVALID_CROC)
                    continue;
                    
                // Find empty slot for new projectile
                for (int j = 0; j < CROC_CAP; j++) {
                    if (projectiles.objs[j].x != INVALID_CROC)
                        continue;

                    // Ensure the shooting crocodile has a valid y-coordinate
                    if (tmp.objs[i].y == INVALID_CROC)
                        break;

                    int lane = (GSIZE/2 - 2 - tmp.objs[i].y - 1) / 2;

                    // Additional validation for lane
                    if (lane < 0 || lane >= NLANES)
                        break;

                    bool moveRight = ((int)isRight + lane) % 2 == 0;

                    projectiles.objs[j].y = tmp.objs[i].y;
                    projectiles.objs[j].x = moveRight ?
                            tmp.objs[i].x + SIZE_CROC - 1 : tmp.objs[i].x;
                    projectiles.objs[j].shoots = true;
                    break;
                }
            }
        } else if (n == -1 && errno != EAGAIN) {
            perror("read");
            // Handle error as needed
        }
 
        // Update existing projectiles
        bool has_active = false;
        for (int i = 0; i < CROC_CAP; i++) {
            if (projectiles.objs[i].x == INVALID_CROC)
                continue;
                
            has_active = true;
            int lane = (GSIZE/2 - 2 - projectiles.objs[i].y - 1) / 2;
            bool moveRight = ((int)isRight + lane) % 2 == 0;
            
            if (moveRight) {
                projectiles.objs[i].x += MOVE_STEP;
                if (projectiles.objs[i].x >= GSIZE)
                    projectiles.objs[i] = invalidateCrocodile(projectiles.objs[i]);
            } else {
                projectiles.objs[i].x -= MOVE_STEP;
                if (projectiles.objs[i].x < 0)
                    projectiles.objs[i] = invalidateCrocodile(projectiles.objs[i]);
            }
        }

        if (has_active) {
            write(pipefd[1], &projectiles, sizeof(msg));
        }
        
        usleep(UDELAY / 4);
    }
}

void printCrocProjectile(WINDOW **g_win, msg p)
{
    for (int i = 0; i < CROC_CAP; i++) {
        if (p.objs[i].x == INVALID_CROC || p.objs[i].y == INVALID_CROC)
            continue;
        // Print projectile at the same level as crocodiles but with distinct character
        mvwaddch(*g_win, p.objs[i].y, p.objs[i].x, SPRITE_PROJECTILE);
    }
}
