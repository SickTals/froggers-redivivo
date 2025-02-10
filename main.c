#include "main.h"
#include "common.h"
#include "curses.h"
#include "frog.h"
#include "river.h"

int main() 
{
    gstate flag = Menu;
    WINDOW *g_win;
    WINDOW *ui_win;
    int lives = 3;
    int score = 0;
    bool dens[NDENS] = {false, false, false, false, false};

    srand(time(NULL));
    init_screen(&g_win, &ui_win);

    while (true) {
        if (flag == Dies) {
            lives--;
            flag = Game;
        }
        if (flag == Win)
            flag = Game;
        if (flag == Menu)
            flag = menu(&g_win);
        if (flag == Exit)
            break;
        if (flag == Game)
            flag = game(&g_win, dens);
    }

    end_screen(&g_win, &ui_win);
    return 0;
}

/*
 * Inizializza e imposta la schermata
 * Le finestre vengono posizionate centrate
 */
void init_screen(WINDOW **g_win, WINDOW **ui_win)
{
    initscr();
    noecho();
    curs_set(false);
    cbreak();
    *g_win = newwin(GSIZE/2, GSIZE,
                    (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) - UISIZE/2);
    *ui_win = newwin(GSIZE/2, UISIZE,
                     (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) + GSIZE - UISIZE/2);
    nodelay(*g_win, false);
    keypad(*g_win, true);
    box(*g_win, ACS_VLINE, ACS_HLINE);
    box(*ui_win, ACS_VLINE, ACS_HLINE);
    wrefresh(*g_win);
    wrefresh(*ui_win);
}

void initObjects(msg msgs[])
{
    msgs[Id_frog].id = Id_frog;
    msgs[Id_frog].p.y = GSIZE/2 - 2;
    msgs[Id_frog].p.x = GSIZE/2;
    for (int i = 0; i < CROC_CAP; i++) {
        msgs[Id_croc_slow].objs[i] = invalidateCrocodile(msgs[Id_croc_slow].objs[i]);
        msgs[Id_croc_normal].objs[i] = invalidateCrocodile(msgs[Id_croc_normal].objs[i]);
        msgs[Id_croc_fast].objs[i] = invalidateCrocodile(msgs[Id_croc_fast].objs[i]);
        msgs[Id_croc_projectile].objs[i] = invalidateCrocodile(msgs[Id_croc_projectile].objs[i]);
    }
}

void child_task(int i, WINDOW **g_win, int pipefd[], int pipefd_projectiles[], int pipefd_grenade[], rvr r)
{
    close(pipefd_projectiles[1]);
    switch (i) {
        case Id_frog:
            close(pipefd_projectiles[0]);
            frog(g_win, pipefd);
            break;
        case Id_granade:
            granade(pipefd, pipefd_grenade);
            break;
        case Id_croc_slow:
        case Id_croc_normal:
        case Id_croc_fast:
            close(pipefd_projectiles[0]);
            river(r, (enum Speeds)i, pipefd);
            break;
        case Id_croc_projectile:
            projectile(pipefd, pipefd_projectiles, r.isRight);
            break;
    }
}

void end_screen(WINDOW **g_win, WINDOW **ui_win)
{
    delwin(*g_win);
    delwin(*ui_win);
    endwin();
}

bool isDrawning(pos f, msg *c, int nspeeds)
{
    if (f.y == GSIZE/2 - 2 || f.y <= GSIZE/2 - 2 - 16)
        return false;
    for (int k = 0; k < nspeeds; k++)
        for (int i = 0; i < CROC_CAP; i++) {
            if (c[k].objs[i].y == INVALID_CROC ||
                c[k].objs[i].x == INVALID_CROC ||
                f.y != c[k].objs[i].y)
                continue;
            if (f.x >= c[k].objs[i].x &&
                f.x <= c[k].objs[i].x + SIZE_CROC - 1)
                return false;
        }
    return true;
}

bool isShot(int proj_active, pos f, msg proj)
{
    for (int i = 0; i < proj_active; i++)
    {
        if (proj.objs[i].y == INVALID_CROC ||
            proj.objs[i].x == INVALID_CROC ||
            f.y != proj.objs[i].y)
            continue;

        if (f.x == proj.objs[i].x) {
            return true;
        }
    }
    return false;
}

bool den(bool dens[NDENS], pos frog_pos) {
    for (int i = 0; i < NDENS; i++) {
        int den_width = 7; // Width of each den sprite
        int start_x = 3 + i + ((GSIZE - 6) / NDENS) * i;
        int end_x = start_x + den_width - 1; // Inclusive
        // Check vertical alignment (rows 1-3)
        if (frog_pos.y < 1 || frog_pos.y > 4) continue;

        // Check if any part of the frog's sprite (3 characters wide) overlaps the den
        if (frog_pos.x > start_x && frog_pos.x < end_x && !dens[i]) {
            dens[i] = true;
            return true; // Exit after first valid collision
        }
    }
    return false;
}

void printDens(WINDOW **win, bool dens[NDENS])
{
	char SPRITE_DEN_OPEN[3][7] = {"SP\"\"YS",
								  "S    S",
								  "S.  .S"};

    char SPRITE_DEN_CLOSE[3][7] = {"SP__YS",
								   "SOOOOS",
								   "S.\"\".S"};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < NDENS; j++)
        {
            if (!dens[j])
                mvwprintw(*win, 2 + i, 3 + j + ((GSIZE - 6) / NDENS) * j, "%s", SPRITE_DEN_OPEN[i]);
            else
                mvwprintw(*win, 2 + i, 3 + j + ((GSIZE - 6) / NDENS) * j, "%s", SPRITE_DEN_CLOSE[i]);

        }
    }

}

gstate collisions(msg msgs[], bool dens[NDENS], int proj_active)
{
    if (isDrawning(msgs[Id_frog].p, &msgs[Id_croc_slow], NSPEEDS)
    || isShot(proj_active, msgs[Id_frog].p, msgs[Id_croc_projectile]))
        return Dies;


    if (den(dens, msgs[Id_frog].p))
        return Win;

    for(int i = 0, j = 0; i < NDENS; i++) {
        if(dens[i])
            j++;
        if(j == NDENS)
            return Exit;
    }

    return Game;
}



gstate game(WINDOW **g_win, bool dens[NDENS])
{
    WINDOW *p_win;
    gstate flag = Game;
    int pipefd[2];
    int pipefd_grenade[2];
    int pipefd_projectiles[2];
    pid_t pids[NTASKS];
    rvr r = generateRiver();

    if (pipe(pipefd) == -1 ||
        pipe(pipefd_projectiles) == -1 ||
        pipe(pipefd_grenade) == -1) {
        perror("Pipe call");
        exit(1);
    }

    p_win = newwin(PSIZE/3, PSIZE,
                  (LINES - PSIZE/3)/2,((COLS - PSIZE) - UISIZE)/2);
    box(p_win, ACS_VLINE, ACS_HLINE);

    for (int i = 0; i < NTASKS; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("Fork fail");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0)
            child_task(i, g_win, pipefd, pipefd_projectiles, pipefd_grenade, r);
    }

    msg msgs[NTASKS + 1];
    initObjects(msgs);
    bool grenade_active = false;  // Add this flag
    int croc_projectiles_active = 0;
    close(pipefd[1]);
    close(pipefd_projectiles[0]);

    while (flag == Game) {
        wclear(*g_win);
        printDens(g_win, dens);
        printCrocs(g_win, &msgs[Id_croc_slow], NSPEEDS);
        printFrog(g_win, msgs[Id_frog]);
        printCrocProjectile(g_win, msgs[Id_croc_projectile]);
        printGranade(g_win, msgs[Id_granade]);
        box(*g_win, ACS_VLINE, ACS_HLINE);
        wrefresh(*g_win);

        flag = collisions(msgs, dens, croc_projectiles_active);
        
        (void)read(pipefd[0], &msgs[NTASKS], sizeof(msgs[NTASKS]));

        switch (msgs[NTASKS].id) {
            case Id_frog:
                msgs[Id_frog] = handleFrog(msgs[NTASKS].p, msgs[Id_frog]);
                if (msgs[NTASKS].shoots && !grenade_active) {
                    msg tmp = msgs[Id_frog];
                    tmp.shoots = true;
                    write(pipefd_grenade[1], &tmp, sizeof(tmp));
                    grenade_active = true;
                }
                break;
            case Id_granade:
                msgs[Id_granade] = msgs[NTASKS];
                if (msgs[Id_granade].p.x >= GSIZE && msgs[Id_granade].sx_x <= 0)
                    grenade_active = false;
                break;
            case Id_croc_slow:
            case Id_croc_normal:
            case Id_croc_fast:
                msgs[msgs[NTASKS].id] = handleCroc(msgs[NTASKS].objs, msgs[msgs[NTASKS].id]);
                // Check shooting crocodiles
                for (int i = 0; i < CROC_CAP; i++) {
                    // Only proceed if this croc exists and is shooting
                    if (msgs[NTASKS].objs[i].x != INVALID_CROC && 
                        msgs[NTASKS].objs[i].y != INVALID_CROC && 
                        msgs[NTASKS].objs[i].shoots) {
                        
                        if (croc_projectiles_active < CROC_CAP) {
                            // Create a new message specifically for the projectile
                            msg projectile_msg;
                            
                            // Copy the shooting crocodile's data
                            projectile_msg.id = Id_croc_projectile;
                            projectile_msg.shoots = true;
                            projectile_msg.objs[0] = msgs[NTASKS].objs[i];
                            projectile_msg.objs[0].shoots = true;
                            
                            // Send to projectile process
                            write(pipefd_projectiles[1], &projectile_msg, sizeof(msg));
                            croc_projectiles_active++;
                        }
                    }
                }
                break;
            case Id_croc_projectile:
                msgs[Id_croc_projectile] = msgs[NTASKS];
                // Update active projectile count
                croc_projectiles_active = 0;
                for (int i = 0; i < CROC_CAP; i++) {
                    if (msgs[Id_croc_projectile].objs[i].x != INVALID_CROC &&
                        msgs[Id_croc_projectile].objs[i].y != INVALID_CROC) {
                        croc_projectiles_active++;
                    }
                }
                break;
            case Id_pause:
                for (int i = 0; i < NTASKS; i++)
                    kill(pids[i], SIGSTOP);

                flushinp();
                PauseMenu(&p_win);

                for (int i = 0; i < NTASKS; i++)
                    kill(pids[i], SIGCONT);
                break;
            case Id_quit:
                flag = Menu;
                for (int i = 0; i < NDENS; i++)
                    dens[i] = false;
                break;
        }
    }

    close(pipefd[0]);
    for (int i = 0; i < NTASKS; i++)
        kill(pids[i], SIGKILL);

    int status;
    pid_t pid;
    do {
        pid = waitpid(-1, &status, WNOHANG); // Non-blocking wait
    } while (pid > 0 || (pid == -1 && errno == EINTR));

    delwin(p_win);
    return flag;
}
