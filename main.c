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

    srand(time(NULL));
    init_screen(&g_win, &ui_win);

    while (true) {
        if (flag == Menu)
            flag = menu(&g_win);
        if (flag == Exit)
            break;
        if (flag == Game)
            flag = game(&g_win);
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
    msgs[Id_frog].shoots = false;
    for (int i = 0; i < CROC_CAP; i++) {
        msgs[Id_croc_slow].crocs[i] = invalidateCrocodile(msgs[Id_croc_slow].crocs[i]);
        msgs[Id_croc_normal].crocs[i] = invalidateCrocodile(msgs[Id_croc_normal].crocs[i]);
        msgs[Id_croc_fast].crocs[i] = invalidateCrocodile(msgs[Id_croc_fast].crocs[i]);
    }
}

void child_task(int i, WINDOW **g_win, int pipefd[], int pipefd_projectiles[], rvr r)
{
    close(pipefd_projectiles[1]);
    switch (i) {
        case Id_frog:
            close(pipefd_projectiles[0]);
            frog(g_win, pipefd);
            break;
        case Id_granade:
            granade(pipefd, pipefd_projectiles);
            break;
        case Id_croc_slow:
            close(pipefd_projectiles[0]);
            river(r, Slow, pipefd);
            break;
        case Id_croc_normal:
            close(pipefd_projectiles[0]);
            river(r, Normal, pipefd);
            break;
        case Id_croc_fast:
            close(pipefd_projectiles[0]);
            river(r, Fast, pipefd);
            break;
    }

}

void end_screen(WINDOW **g_win, WINDOW **ui_win)
{
    delwin(*g_win);
    delwin(*ui_win);
    endwin();
}


int game(WINDOW **g_win)
{
    WINDOW *p_win;
    gstate flag = Game;
    int pipefd[2];
    int pipefd_projectiles[2];
    pid_t pids[NTASKS];
    rvr r = generateRiver();

    if (pipe(pipefd) == -1 || pipe(pipefd_projectiles) == -1) {
        perror("Pipe call");
        exit(1);
    }

    p_win = newwin(PSIZE/3, PSIZE, (LINES - PSIZE/3)/2, ((COLS - PSIZE) - UISIZE)/2);
    box(p_win, ACS_VLINE, ACS_HLINE);

    for (int i = 0; i < NTASKS; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("Fork fail");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0)
            child_task(i, g_win, pipefd, pipefd_projectiles, r);
    }

    msg msgs[NTASKS + 1];
    initObjects(msgs);
    bool grenade_active = false;  // Add this flag
    close(pipefd[1]);
    close(pipefd_projectiles[0]);

    while (flag == Game) {
        
        wclear(*g_win);
        printCrocs(g_win, msgs[Id_croc_slow].crocs,
                   msgs[Id_croc_normal].crocs, msgs[Id_croc_fast].crocs);
        printFrog(g_win, msgs[Id_frog]);
        printGranade(g_win, msgs[Id_granade]);
        box(*g_win, ACS_VLINE, ACS_HLINE);
        wrefresh(*g_win);
        
        (void)read(pipefd[0], &msgs[NTASKS], sizeof(msgs[NTASKS]));

        switch (msgs[NTASKS].id) {
            case Id_frog:
                msgs[Id_frog] = handleFrog(msgs[NTASKS].p, msgs[Id_frog]);
                if (msgs[NTASKS].shoots && !grenade_active) {    
                    msg shoot_msg = msgs[Id_frog];
                    shoot_msg.shoots = true;
                    (void)write(pipefd_projectiles[1], &shoot_msg, sizeof(shoot_msg));
                    grenade_active = true;
                }
                break;
            case Id_granade:
                msgs[Id_granade] = msgs[NTASKS];
                if (msgs[Id_granade].p.x >= GSIZE && msgs[Id_granade].sx_x <= 0)
                    grenade_active = false;
                break;
            case Id_croc_slow:
                msgs[Id_croc_slow] = handleCroc(msgs[NTASKS].crocs, msgs[Id_croc_slow]);
                break;
            case Id_croc_normal:
                msgs[Id_croc_normal] = handleCroc(msgs[NTASKS].crocs, msgs[Id_croc_normal]);
                break;
            case Id_croc_fast:
                msgs[Id_croc_fast] = handleCroc(msgs[NTASKS].crocs, msgs[Id_croc_fast]);
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
