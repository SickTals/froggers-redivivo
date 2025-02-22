#include "ui.h"
#include "common.h"
#include "curses.h"
#include "river.h"

void printMenuUi(WINDOW **win)
{
    char sprite[LENGHT_STITLE][WIDTH_STITLE] = {SPRITE_STITLE};

    wclear(*win);
    for(int j = 1; j < UISIZE - 1; j++)
        for (int i = 1; i < GSIZE/2 - 1; i++)
            mvwaddch(*win, i, j, ' ');

    bool flag = COIN_FLIP;
    wattron(*win, COLOR_PAIR(Evil_Ui));
    box(*win, ACS_VLINE, ACS_HLINE);
    for(int i = 0, j = 0; i < LENGHT_STITLE; i++) {
        if (j == WIDTH_STITLE) {
            j = 1;
            if(flag)
                flag = false;
            else
                flag = true;
        } else {
            j++;
        }

        if (flag) {
            mvwprintw(*win, i + SIZE_PIXEL, START_X + 1 , "%s", sprite[i]);
        } else {
            mvwprintw(*win, i + SIZE_PIXEL, START_X, "%s", sprite[i]);
        }
    }
    wattroff(*win, COLOR_PAIR(Evil_Ui));

    wrefresh(*win);
}

void timer(int pipefd[])
{
    msg t = {
        t.id = Id_timer
    };

    close(pipefd[0]);

    for (int i = TIME; i >= 0; i--) {
        t.objs[Id_timer].y = i;
        (void)write(pipefd[1], &t, sizeof(t));
        sleep(1);
    }
    t.id = Id_quit;
    (void)write(pipefd[1], &t, sizeof(t));

    close(pipefd[1]);
    _exit(0);
}


void printUi(WINDOW **win, msg t, int lives, int score)
{
    char fscore[SIZE_FSCORE + 1];

    snprintf(fscore, sizeof(fscore), "%016d", score);

    wattron(*win, COLOR_PAIR(Ui));
    for(int j = 1; j < UISIZE - 1; j++)
        for (int i = 1; i < GSIZE/2 - 1; i++)
            mvwaddch(*win, i, j, ' ');

    wattron(*win, COLOR_PAIR(Lives));
    mvwprintw(*win, TIMER_Y - 1, START_X, "TIME");
    mvwprintw(*win, TIMER_Y, UI_CENTER_X, "%d", t.objs[Id_timer].y);
    mvwprintw(*win, LIVES_Y - 1, START_X, "LIVE");
    for (int i = 0; i < lives; i++)
        mvwprintw(*win, LIVES_Y + i, UI_CENTER_X - (i % 2), "%s", SPRITE_LIFE);

    mvwprintw(*win, SCORE_Y - 1, (START_X - 1), "POINTS");
    for (int i = SIZE_FSCORE; i >= 0; i--)
        mvwprintw(*win, SCORE_Y + i, UI_CENTER_X + (i % 2), "%c", fscore[i]);

    wattroff(*win, COLOR_PAIR(Lives));
    wattroff(*win, COLOR_PAIR(Ui));
}
