#include "main.h"
#include "common.h"
#include "frog.h"
#include "menu.h"
#include "river.h"


int DIFFICULTY = 2; // Default: normale
int TIME = 99;
int SHOOT_CHANCE = 25;
SharedData shared_data;
static msg buffer[DIM_BUFFER];
static int buffer_counter = 0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

// Thread-safe write
void write_message(msg m) {
    pthread_mutex_lock(&lock);
    // Wait until buffer has space
    while (buffer_counter == DIM_BUFFER) {
        pthread_cond_wait(&buffer_not_full, &lock);
    }
    // Add message to buffer (FIFO)
    buffer[buffer_counter++] = m;
    // Signal that buffer is no longer empty
    pthread_cond_signal(&buffer_not_empty);
    pthread_mutex_unlock(&lock);
}

// Thread-safe read
msg read_message() {
    pthread_mutex_lock(&lock);
    // Wait until buffer has data
    while (buffer_counter == 0) {
        pthread_cond_wait(&buffer_not_empty, &lock);
    }
    // Read message (FIFO)
    msg m = buffer[0];
    // Shift remaining messages forward
    for (int i = 1; i < buffer_counter; i++) {
        buffer[i - 1] = buffer[i];
    }
    buffer_counter--;
    // Signal that buffer is no longer full
    pthread_cond_signal(&buffer_not_full);
    pthread_mutex_unlock(&lock);
    return m;
}

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
                flag = menu(&g_win, &ui_win, Msg_play, Menu);
                break;
            case Exit:
                flag = Exit;
                break;
            case Game:
                flag = game(&g_win, &ui_win, lives, score, dens);
                break;
            case EndW:
                for (int h = 0; h < 10; h++)
                    end_screen(&g_win, dens, h, (const char*[]){SPRITE_WIN}, Evil_Ui);
                flag = Menu;
                break;
            case EndL:
                for (int h = 0; h < 10; h++)
                    end_screen(&g_win, dens, h, (const char*[]){SPRITE_LOSE}, Alt_E_Ui);
                flag = Menu;
                break;
            case Options:
                flag = menu(&g_win, &ui_win, Msg_difficulty, Options);
                break;
            case SprOpt:
                flag = menu(&g_win, &ui_win, Pair_one, SprOpt);
                break;
            case DiffOpt:
                flag = menu(&g_win, &ui_win, Diff_easy, DiffOpt);
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
    if(h % 2) {
        for (int i = 0; i < LENGHT_PAUSE; i++) {
            mvwprintw(*win, (CENTER_X / 2) - 3 + i, CENTER_X - strlen(sprite[i]) / 2,
                      "%s", sprite[i]);
        }
    } else {
        wattron(*win, COLOR_PAIR(alt_color));
        for (int i = 0; i < LENGHT_PAUSE; i++) {
            mvwprintw(*win, (CENTER_X / 2) - 3 + i, CENTER_X - strlen(sprite[i]) / 2,
                      "%s", sprite[i]);
        }
        wattroff(*win, COLOR_PAIR(alt_color));
    }
    wattroff(*win, COLOR_PAIR(Ui));
    wrefresh(*win);
    usleep(UDELAY/5);
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
    init_pair(Grass, COLOR_WHITE, COLOR_GREEN);
    init_pair(Frog, COLOR_WHITE, COLOR_GREEN);
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
    msgs[Id_granade].objs[Idx_sx_grenade].x = 0;
    msgs[Id_granade].objs[Idx_dx_grenade].x = GSIZE;
    for (int i = Slow; i <= Id_croc_projectile; i++)
        for (int j = 0; j < CROC_CAP; j++)
            msgs[i].objs[j] = invalidateObject();
}

void* thread_task(void* arg){
    int task_id = (intptr_t)arg; // Cast task ID from void*
    switch (task_id) {
        case Id_frog:
            frog(g_win); // Remove pipefd dependency
            break;
        case Id_granade:
            granade(); // Remove pipefd dependency
            break;
        case Id_croc_slow:
        case Id_croc_normal:
        case Id_croc_fast:
            river(task_id, r); // Pass river data
            break;
        case Id_croc_projectile:
            projectile(r.isRight);
            break;
        case Id_timer:
            timer();
            break;
        default:
            break;
    }
    return NULL;
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
    wattron(*win, COLOR_PAIR(Grass));
    for(int i = 1; i < GSIZE - 1; i++) {
        mvwaddch(*win, SIDEWALK_Y - 1, i, ' ');
        mvwaddch(*win, SIDEWALK_Y, i, ' ');
    }

    for(int j = 1; j <= NLANES; j++)
        for(int i = 1; i < GSIZE - 1; i++)
            mvwaddch(*win, j, i, ' ');
    wattroff(*win, COLOR_PAIR(Grass));

    wattron(*win, COLOR_PAIR(River));
    for(int j = NLANES + 1; j < SIDEWALK_Y - 1; j++)
        for(int i = 1; i < GSIZE - 1; i++)
            mvwaddch(*win, j, i, ' ');
    wattroff(*win, COLOR_PAIR(River));
}

void grenadeCollision(msg g, msg p, int pipefd_projectiles[], int pipefd_grenade[])
{
    msg tmp_g = {
        .objs[Idx_dx_grenade].x = 0,
        .objs[Idx_sx_grenade].x = 0
    };
    msg tmp_p = {
        .objs[CROC_CAP - 1].x = INVALID_CROC
    };

    for (int i = 0; i < CROC_CAP - 1; i++) {
        if (g.objs[Idx_sx_grenade].x == p.objs[i].x &&
            g.objs[Idx_sx_grenade].y == p.objs[i].y) {
            tmp_g.objs[Idx_sx_grenade].x = INVALID_CROC;
        } else if (g.objs[Idx_dx_grenade].x == p.objs[i].x &&
                   g.objs[Idx_dx_grenade].y == p.objs[i].y) {
            tmp_g.objs[Idx_dx_grenade].x = INVALID_CROC;
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

msg handleObject(msg msgs[NTASKS + 1], int pipefd_grenade[], int pipefd_projectiles[], bool *grenade_active, int *croc_projectiles_active, bool isRight)
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
            for (int i = 0; i < CROC_CAP; i++) {
                if (!(IS_CROC_SHOOTING))
                    continue;
                *croc_projectiles_active = sendProjectileShot(pipefd_projectiles,
                                                              msgs[NTASKS].objs[i],
                                                              *croc_projectiles_active);
            }
            msgs[Id_frog].objs[0].x += slideWithCroc(msgs[Id_frog].objs[0],
                                                     msgs[msgs[NTASKS].id].objs, isRight);
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
    WINDOW *p_win = newwin(PSIZE/3, PSIZE, PWIN_START_Y, PWIN_START_X);
    gstate flag = Game;
    pid_t pids[NTASKS];
    int pipefd[2];
    int pipefd_grenade[2];
    int pipefd_projectiles[2];
    rvr r = generateRiver();
    bool grenade_active = false;
    int croc_projectiles_active = 0;

    pthread_t threads[NTASKS];
    for (int i = 0; i < NTASKS; i++) {
        pthread_create(&threads[i], NULL, thread_task, (void*)(intptr_t)i);
    }
    initObjects(shared_data.msgs);
    close(pipefd[1]);
    close(pipefd_projectiles[0]);
    close(pipefd_grenade[0]);
    wclear(*g_win);
    wclear(*ui_win);

    while (flag == Game) {
        printUi(ui_win, shared_data.msgs[Id_timer], lives, score);
        init_bckg(g_win);
        printDens(g_win, dens);
        printCrocs(g_win, &shared_data.msgs[Id_croc_slow], NSPEEDS, r.isRight);
        printFrog(g_win, shared_data.msgs[Id_frog].objs[0]);
        printCrocProjectile(g_win, shared_data.msgs[Id_croc_projectile]);
        printGranade(g_win, shared_data.msgs[Id_granade].objs);
        wattron(*ui_win, COLOR_PAIR(Ui));
        wattron(*g_win, COLOR_PAIR(Ui));
        box(*g_win, ACS_VLINE, ACS_HLINE);
        box(*ui_win, ACS_VLINE, ACS_HLINE);
        wattroff(*ui_win, COLOR_PAIR(Ui));
        wattroff(*g_win, COLOR_PAIR(Ui));

        wrefresh(*g_win);
        wrefresh(*ui_win);

        flag = collisions(shared_data.msgs, dens, r.isRight, pipefd_projectiles, pipefd_grenade, croc_projectiles_active);

        msg incoming = read_message();
        pthread_mutex_lock(&shared_data.mutex);
        shared_data.messages[incoming.id] = incoming;
        pthread_mutex_unlock(&shared_data.mutex);

        if (lives <= 0) {
            flag = EndL;
        } else if (shared_data.msgs[NTASKS].id == Id_pause) {
            for (int i = 0; i < NTASKS; i++)
                kill(pids[i], SIGSTOP);
            flushinp();
            pauseMenu(&p_win);
            for (int i = 0; i < NTASKS; i++)
                kill(pids[i], SIGCONT);
        } else if (shared_data.msgs[NTASKS].id == Id_quit) {
            flag = Menu;
        }

        shared_data.msgs[shared_data.msgs[NTASKS].id] = handleObject(shared_data.msgs, pipefd_grenade, pipefd_projectiles,
                                             &grenade_active, &croc_projectiles_active,
                                             r.isRight);
    }

    wclear(*g_win);
    wclear(*ui_win);
    wrefresh(*g_win);
    wrefresh(*ui_win);

    for (int i = 0; i < NTASKS; i++) {
        pthread_cancel(threads[i]); // Gracefully terminate
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&shared_data.mutex);
    pthread_cond_destroy(&shared_data.cond);

    delwin(p_win);
    return flag;
}
