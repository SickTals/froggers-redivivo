#include "main.h"
#include "common.h"
#include "frog.h"
#include "river.h"

int main() 
{
    gstate flag = Menu;
    WINDOW *g_win;
    WINDOW *ui_win;
    int lives = NLIVES;
    int score = 0;
    bool dens[NDENS] = {false};

    srand(time(NULL));
    init_screen(&g_win, &ui_win);

    while (flag != Exit) {
        switch (flag) {
            case Dies:
                lives--;
                flag = Game;
                break;
            case Win:
                score += 1000;
                flag = Game;
                break;
            case Menu:
                lives = NLIVES;
                score = 0;
                for (int i = 0; i < NDENS; i++)
                    dens[i] = false;
                flag = menu(&g_win, &ui_win);
                break;
            case Exit:
                flag = Exit;
                break;
            case Game:
                flag = game(&g_win, &ui_win, lives, score, dens);
                break;
            case EndW:
                for (int h = 0; h < 10; h++) {
                    end_screen(&g_win, dens, h, (const char*[]){SPRITE_WIN}, Evil_Ui);
                    wrefresh(g_win);
                    usleep(UDELAY/5);
                }
                flag = Menu;
                break;
            case EndL:
                for (int h = 0; h < 10; h++) {
                    end_screen(&g_win, dens, h, (const char*[]){SPRITE_LOSE}, Alt_E_Ui);
                    wrefresh(g_win);
                    usleep(UDELAY/5);
                }
                flag = Menu;
                break;
            default:
                flag = Exit;
                break;
        }
    }
    kill_screen(&g_win, &ui_win, dens);

    return 0;
}

void end_screen(WINDOW **win, bool dens[], int h, const char *sprite[], int alt_color) {
    wattron(*win, COLOR_PAIR(Ui));
    box(*win, ACS_VLINE, ACS_HLINE);
    for(int j = 1; j < GSIZE/2 - 1; j++)
        for (int i = 1; i < GSIZE - 1; i++)
            mvwaddch(*win, j, i, ' ');
    switch((h % 2)){
        case 1:
            for (int i = 0; i < 5; i++)
                mvwprintw(*win, (CENTER_X / 2) - 3 + i, CENTER_X - strlen(sprite[i]) / 2, "%s", sprite[i]);
            break;
        default:
            wattron(*win, COLOR_PAIR(alt_color));
            for (int i = 0; i < 5; i++)
                mvwprintw(*win, (CENTER_X / 2) - 3 + i, CENTER_X - strlen(sprite[i]) / 2, "%s", sprite[i]);
            wattroff(*win, COLOR_PAIR(alt_color));
    }
    wattroff(*win, COLOR_PAIR(Ui));
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
    start_color();
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
    init_color(COLOR_BROWN, 150, 75, 0);
    init_pair(Grass_Frog, COLOR_WHITE, COLOR_GREEN);
    init_pair(River, COLOR_WHITE, COLOR_CYAN);
    init_pair(Crocs, COLOR_BLACK, COLOR_BLUE);
    init_pair(Dens, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(Gren, COLOR_BLACK, COLOR_GREEN);
    init_pair(Ui, COLOR_YELLOW, COLOR_BROWN);
    init_pair(Lives, COLOR_RED, COLOR_WHITE);
    init_pair(Evil_Ui, COLOR_RED, COLOR_BROWN);
    init_pair(Alt_E_Ui, COLOR_CYAN, COLOR_BROWN);
}

void initObjects(msg msgs[])
{
    msgs[Id_timer].objs[Id_timer].y = TIME;
    msgs[Id_frog].objs[0].y = SIDEWALK_Y;
    msgs[Id_frog].objs[0].x = CENTER_X;
    msgs[Id_granade].objs[1].x = GSIZE;
    msgs[Id_granade].objs[0].x = 0;
    for (int i = Slow; i <= Id_croc_projectile; i++)
        for (int j = 0; j < CROC_CAP; j++)
            msgs[i].objs[j] = invalidateObject();
}

void child_task(int i, WINDOW **g_win, int pipefd[], int pipefd_projectiles[], int pipefd_grenade[], rvr r)
{
    close(pipefd_projectiles[1]);
    close(pipefd_grenade[1]);
    switch (i) {
        case Id_frog:
            close(pipefd_grenade[0]);
            close(pipefd_projectiles[0]);
            frog(g_win, pipefd);
            break;
        case Id_granade:
            close(pipefd_projectiles[0]);
            granade(pipefd, pipefd_grenade);
            break;
        case Id_croc_slow:
        case Id_croc_normal:
        case Id_croc_fast:
            close(pipefd_grenade[0]);
            close(pipefd_projectiles[0]);
            river(r, (enum Speeds)i, pipefd);
            break;
        case Id_croc_projectile:
            close(pipefd_grenade[0]);
            projectile(pipefd, pipefd_projectiles, r.isRight);
            break;
       case Id_timer:
            close(pipefd_grenade[0]);
            close(pipefd_projectiles[0]);
            timer(pipefd);
            break;
    }
}


void kill_screen(WINDOW **g_win, WINDOW **ui_win, bool dens[NDENS])
{
    delwin(*g_win);
    delwin(*ui_win);
    endwin();
}

bool isDrawning(obj f, msg *c, int nspeeds)
{
    if (f.y >= SIDEWALK_Y || f.y <= SIDEWALK_Y - BOX_BORDER * 2 - (NLANES * SIZE_PIXEL))
        return false;
    for (int k = 0; k < nspeeds; k++) {
        for (int i = 0; i < CROC_CAP; i++) {
            if (c[k].objs[i].y == INVALID_CROC ||
                c[k].objs[i].x == INVALID_CROC ||
                f.y != c[k].objs[i].y) {
                continue;
            }
            if (f.x >= c[k].objs[i].x &&
                f.x < c[k].objs[i].x + SIZE_CROC) {
                return false;
            }
        }
    }
    return true;
}

bool isShot(int proj_active, obj frog, msg proj)
{
    for (int i = 0; i < proj_active; i++) {
        if (proj.objs[i].y == INVALID_CROC ||
            proj.objs[i].x == INVALID_CROC ||
            frog.y != proj.objs[i].y) {
            continue;
        }

        if (proj.objs[i].x >= frog.x && proj.objs[i].x < frog.x + WIDTH_FROG)
            return true;
    }
    return false;
}

gstate hasWon(bool dens[NDENS])
{
    for(int i = 0, j = 0; i < NDENS; i++) {
        if(dens[i])
            j++;
        if(j == NDENS)
            return true;
    }
    return false;
}

void init_bckg(WINDOW **win)
{
    wattron(*win, COLOR_PAIR(Grass_Frog));
    for(int i = 1; i < GSIZE - 1; i++) {
        mvwaddch(*win, SIDEWALK_Y - 1, i, ' ');
        mvwaddch(*win, SIDEWALK_Y, i, ' ');
    }

    for(int j = 1; j <= NLANES; j++)
        for(int i = 1; i < GSIZE - 1; i++)
            mvwaddch(*win, j, i, ' ');
    wattroff(*win, COLOR_PAIR(Grass_Frog));

    wattron(*win, COLOR_PAIR(River));
    for(int j = NLANES + 1; j < SIDEWALK_Y - 1; j++)
        for(int i = 1; i < GSIZE - 1; i++)
            mvwaddch(*win, j, i, ' ');
    wattroff(*win, COLOR_PAIR(River));
}

void grenadeCollision(msg g, msg p, int pipefd_projectiles[], int pipefd_grenade[])
{
    msg tmp_g = {
        .objs[1].x = 0,
        .objs[0].x = 0
    };
    msg tmp_p = {
        .objs[CROC_CAP - 1].x = INVALID_CROC
    };

    for (int i = 0; i < CROC_CAP - 1; i++) {
        if (g.objs[0].x == p.objs[i].x && g.objs[0].y == p.objs[i].y) {
            tmp_g.objs[0].x = INVALID_CROC;
        } else if (g.objs[1].x == p.objs[i].x && g.objs[1].y == p.objs[i].y) {
            tmp_g.objs[1].x = INVALID_CROC;
        } else {
            continue;
        }
        tmp_p.objs[CROC_CAP - 1].y = i;
        write(pipefd_grenade[1], &tmp_g, sizeof(msg));
        write(pipefd_projectiles[1], &tmp_p, sizeof(msg));
    }
}

gstate collisions(msg msgs[], bool dens[NDENS], bool isRight, int pipefd_projectiles[], int pipefd_grenade[], int proj_active)
{
    if (isDrawning(msgs[Id_frog].objs[0], &msgs[Id_croc_slow], NSPEEDS) ||
        isShot(proj_active, msgs[Id_frog].objs[0], msgs[Id_croc_projectile])) {
        return Dies;
    }
    if (den(dens, msgs[Id_frog].objs[0]))
        return Win;
    if (hasWon(dens))
        return EndW;

    grenadeCollision(msgs[Id_granade], msgs[Id_croc_projectile], pipefd_projectiles, pipefd_grenade);

    return Game;
}

bool sendGrenadeShot(int pipefd[], msg f)
{
    msg tmp = f;
    tmp.objs[0].shoots = true;
    write(pipefd[1], &tmp, sizeof(tmp));
    return true;
}


int sendProjectileShot(int pipefd[], obj c, int n)
{
    // Create a new message specifically for the projectile
    msg projectile_msg = {
                        .id = Id_croc_projectile,
                        .objs[CROC_CAP - 1] = c ,
    };
    write(pipefd[1], &projectile_msg, sizeof(msg));
    return n + 1;
}

int updateProjectileCount(obj p[])
{
    int n = 0;
    for (int i = 0; i < CROC_CAP - 1; i++) {
        if (p[i].x != INVALID_CROC && p[i].y != INVALID_CROC)
            n++;
    }
    return n;

}

msg handleObject(msg msgs[NTASKS + 1], int pipefd_grenade[], int pipefd_projectiles[], bool *grenade_active, int *croc_projectiles_active)
{
    switch (msgs[NTASKS].id) {
        case Id_frog:
            if (msgs[NTASKS].objs[0].shoots && !(*grenade_active))
                *grenade_active = sendGrenadeShot(pipefd_grenade, msgs[Id_frog]);
            return handleFrog(msgs[NTASKS].objs[0], msgs[Id_frog].objs[0]);
        case Id_granade:
            if (msgs[NTASKS].objs[1].x >= GSIZE && msgs[NTASKS].objs[0].x <= 0)
                *grenade_active = false;
            return msgs[NTASKS];
        case Id_croc_slow:
        case Id_croc_normal:
        case Id_croc_fast:
            // Check shooting crocodiles
            for (int i = 0; i < CROC_CAP; i++) {
                // Only proceed if this croc exists and is shooting
                if (!(IS_CROC_SHOOTING))
                    continue;
                *croc_projectiles_active = sendProjectileShot(pipefd_projectiles, msgs[NTASKS].objs[i], *croc_projectiles_active);
            }
            return handleCroc(msgs[NTASKS].objs, msgs[msgs[NTASKS].id]);
        case Id_croc_projectile:
            *croc_projectiles_active = updateProjectileCount(msgs[NTASKS].objs);
            return msgs[NTASKS];
        case Id_timer:
            return msgs[NTASKS];
        default:
            return msgs[msgs[NTASKS].id];
    }
}

gstate game(WINDOW **g_win, WINDOW **ui_win, int lives, int score, bool dens[NDENS])
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

    p_win = newwin(PSIZE/3, PSIZE, PWIN_START_Y, PWIN_START_X);
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
    close(pipefd_grenade[0]);

    while (flag == Game) {
        wclear(*g_win);
        wclear(*ui_win);

        printUi(ui_win, msgs[Id_timer], lives, score);
        init_bckg(g_win);
        printDens(g_win, dens);
        printCrocs(g_win, &msgs[Id_croc_slow], NSPEEDS, r.isRight);
        printFrog(g_win, msgs[Id_frog].objs[0]);
        printCrocProjectile(g_win, msgs[Id_croc_projectile]);
        printGranade(g_win, msgs[Id_granade].objs);
        wattron(*ui_win, COLOR_PAIR(Ui));
        wattron(*g_win, COLOR_PAIR(Ui));
        box(*g_win, ACS_VLINE, ACS_HLINE);
        box(*ui_win, ACS_VLINE, ACS_HLINE);
        wattroff(*ui_win, COLOR_PAIR(Ui));
        wattroff(*g_win, COLOR_PAIR(Ui));

        wrefresh(*g_win);
        wrefresh(*ui_win);

        flag = collisions(msgs, dens, r.isRight, pipefd_projectiles, pipefd_grenade, croc_projectiles_active);
        
        (void)read(pipefd[0], &msgs[NTASKS], sizeof(msgs[NTASKS]));

        if (lives <= 0) {
            flag = EndL;
        } else if (msgs[NTASKS].id == Id_pause) {
            for (int i = 0; i < NTASKS; i++)
                kill(pids[i], SIGSTOP);
            flushinp();
            pauseMenu(&p_win);
            for (int i = 0; i < NTASKS; i++)
                kill(pids[i], SIGCONT);
        } else if (msgs[NTASKS].id == Id_quit) {
            flag = Menu;
        }

        msgs[msgs[NTASKS].id] = handleObject(msgs, pipefd_grenade, pipefd_projectiles, &grenade_active, &croc_projectiles_active);
    }

    wclear(*g_win);
    wclear(*ui_win);
    wrefresh(*g_win);
    wrefresh(*ui_win);

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
