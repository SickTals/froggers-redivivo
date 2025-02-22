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

obj invalidateObject()
{
    obj c = {
        .y= INVALID_CROC,
        .x= INVALID_CROC,
        .shoots = false
    };
    return c;
}

msg initCrocodiles(enum Speeds speed)
{
    msg croc = {
        .id = (msgid)speed
    };
    // Initialize all positions as invalid
    for (int i = 0; i < CROC_CAP; i++)
        croc.objs [i] = invalidateObject();

    return croc;
}

obj updateCrocodilePosition(obj croc, bool moveRight)
{
    croc.shoots = SHOOT_CHANCE;

    if (moveRight) {
        croc.x += MOVE_STEP;
        if (croc.x >= GSIZE)
            croc = invalidateObject();
    } else {
        croc.x -= MOVE_STEP;
        if (croc.x < -(SIZE_CROC + 1))
            croc = invalidateObject();
    }
    return croc;
}

obj updateCrocodile(obj c, bool isRight, int *active_crocs)
{
    if (c.x == INVALID_CROC || c.y == INVALID_CROC)
        return c;

    int lane = CALC_LANE(c.y);
    
    if (!(IS_VALID_LANE(lane))) {
        c = invalidateObject();
        return c;
    }
    
    bool moveRight = CALC_DIRECTION(isRight, lane);
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
        c[i] = invalidateObject();
    }
    *validIdx += 1;
    return c[i];
}

obj spawnCrocodile(obj c[], int lane, bool isRight, int validIdx)
{
    int lane_y = GSIZE/2 - 2 - (1 + lane) * 2;
    bool moveRight = CALC_DIRECTION(isRight, lane);
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
        c.objs[i] = invalidateObject();

    // Copy valid positions
    int validIdx = 0;
    for (int i = 0; i < CROC_CAP; i++) {
        if (p[i].x == INVALID_CROC || p[i].y == INVALID_CROC)
            continue;
        c.objs[validIdx] = p[i];
        validIdx++;
    }
    
    if (c.objs[CROC_CAP - 1].shoots)
        c.id = Id_croc_projectile;

    return c;
}

int slideWithCroc(obj f, obj c[], bool isRight) {
    if (f.y >= GSIZE / 2 - 1 || f.y <= (GSIZE / 2) - 2 - (NLANES * SIZE_PIXEL))
        return 0;

    for (int i = 0; i < CROC_CAP; i++) {
        if (c[i].y == INVALID_CROC ||
            c[i].x == INVALID_CROC ||
            f.y != c[i].y)
            continue;

        if (f.x >= c[i].x && f.x < c[i].x + SIZE_CROC) {
            bool moveRight = CALC_DIRECTION(isRight, CALC_LANE(c[i].y));
            return (moveRight ? 1 : -1);
        }
    }
    return 0;
}

void printCrocs(WINDOW **g_win, msg *c, int nspeeds, bool isRight)
{
    char sprite_left[SIZE_PIXEL][SIZE_CROC] = {SPRITE_CROC_LEFT};
    char sprite_right[SIZE_PIXEL][SIZE_CROC] = {SPRITE_CROC_RIGHT};

    wattron(*g_win, COLOR_PAIR(Crocs));
    for (int s = 0; s < nspeeds; s++)
    {
        for (int j = 0; j < CROC_CAP; j++) {
            if (c[s].objs[j].x == INVALID_CROC || c[s].objs[j].y == INVALID_CROC)
                break;
            int lane = CALC_LANE(c[s].objs[j].y);
            bool moveRight = CALC_DIRECTION(isRight, lane);
            for (int i = 0; i < SIZE_CROC; i++) {
                if (!(IS_ON_SCREEN(c[s].objs[j].x + i)))
                    continue;

                mvwaddch(*g_win, c[s].objs[j].y - 1, c[s].objs[j].x + i,
                         moveRight ? sprite_right[0][i] : sprite_left[0][i]);
                mvwaddch(*g_win, c[s].objs[j].y, c[s].objs[j].x + i,
                         moveRight ? sprite_right[1][i] : sprite_left[1][i]);
            }
        }
    }
    wattroff(*g_win, COLOR_PAIR(Crocs));
}


void projectile(int pipefd[], int pipefd_projectiles[], bool isRight) {
    msg projectiles = {
        .id = Id_croc_projectile
    };
    
    // Initialize all projectiles as invalid
    for (int i = 0; i < CROC_CAP - 1; i++)
        projectiles.objs[i] = invalidateObject();

    // Set non-blocking read
    int flags = fcntl(pipefd_projectiles[0], F_GETFL, 0);
    fcntl(pipefd_projectiles[0], F_SETFL, flags | O_NONBLOCK);
    
    close(pipefd[0]);
    
    while (true) {
        msg tmp = { .objs[CROC_CAP - 1].x = 0 };
        ssize_t n = read(pipefd_projectiles[0], &tmp, sizeof(msg));
        if (n == sizeof(msg)) {
            // tentativo di eliminare i proiettili colpiti
            if (tmp.objs[CROC_CAP - 1].x == INVALID_CROC) {
                projectiles.objs[tmp.objs[CROC_CAP - 1].y] = invalidateObject();
            }

            // Handle new shooting crocodiles
            if (tmp.objs[CROC_CAP - 1].shoots) {
                for (int i = 0; i < CROC_CAP; i++) {
                    if (!tmp.objs[i].shoots || tmp.objs[i].x == INVALID_CROC)
                        continue;
                        
                    // Find empty slot for new projectile
                    for (int j = 0; j < CROC_CAP - 1; j++) {
                        if (projectiles.objs[j].x != INVALID_CROC)
                            continue;
                            
                        int lane = (GSIZE/2 - 2 - tmp.objs[i].y - 1) / 2;
                        if (tmp.objs[i].y == INVALID_CROC ||
                            lane < 0 ||
                            lane >= NLANES) {
                            break;
                        }
                        bool moveRight = CALC_DIRECTION(isRight, lane);
                        
                        projectiles.objs[j].y = tmp.objs[i].y;
                        projectiles.objs[j].x = moveRight ? 
                            tmp.objs[i].x + SIZE_CROC - 1 : tmp.objs[i].x;
                        projectiles.objs[j].shoots = true;
                        break;
                    }
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
            bool moveRight = CALC_DIRECTION(isRight, CALC_LANE(projectiles.objs[i].y));
            
            if (moveRight) {
                projectiles.objs[i].x += MOVE_STEP;
                if (projectiles.objs[i].x >= GSIZE)
                    projectiles.objs[i] = invalidateObject();
            } else {
                projectiles.objs[i].x -= MOVE_STEP;
                if (projectiles.objs[i].x < 0)
                    projectiles.objs[i] = invalidateObject();
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
    char sprite_proj[SIZE_PIXEL]= {SPRITE_PROJ};
    wattron(*g_win, COLOR_PAIR(Crocs));
    for (int i = 0; i < CROC_CAP; i++) {
        if (p.objs[i].x == INVALID_CROC || p.objs[i].y == INVALID_CROC)
            continue;
        // Print projectile at the same level as crocodiles but with distinct character
        mvwaddch(*g_win, p.objs[i].y - 1, p.objs[i].x, sprite_proj[0]);
        mvwaddch(*g_win, p.objs[i].y, p.objs[i].x, sprite_proj[1]);
    }
    wattroff(*g_win, COLOR_PAIR(Crocs));

}
