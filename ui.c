#include "ui.h"
#include "common.h"
#include "curses.h"

void printMenuUi(WINDOW **win)
{

    char sprite[24][4] = {
            "[*)",
            "|\\_",
            "[__",
            "[__",
            "/_",
            " _/",
            "| |",
            "|_|",
            "[*)",
            "|\\ ",
            "[*)",
            "|\\_",
            "[__",
            "[__",
            "/  ",
            "\\__",
            "___",
            " | ",
            " ' ",
            " | ",
            "/ \\",
            "\\_/",
            "|\\|",
            "| \\"
    };

    wclear(*win);
    box(*win, ACS_VLINE, ACS_HLINE);
    for(int i = 0, j = 0, flag = 1 ; i < 24; i++) {
        if (j == 4) {
            j = 1;
            if(flag)
                flag = 0;
            else
                flag = 1;
        }
        else
            j++;

        switch (flag) {
            case 1:
                mvwprintw(*win, i+2, START_X + 1 , "%s", sprite[i]);
                break;
            default:
                mvwprintw(*win, i+2, START_X, "%s", sprite[i]);
                break;
        }
    }
    wrefresh(*win);
}

void timer(int pipefd[])
{
    close(pipefd[0]);

    msg t;
    t.id = Id_timer;

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
    mvwprintw(*win, TIMER_Y - 1, START_X, "TIME");
    mvwprintw(*win, TIMER_Y, UI_CENTER_X, "%d", t.objs[Id_timer].y);
    mvwprintw(*win, LIVES_Y - 1, START_X, "LIVE");
    mvwprintw(*win, SCORE_Y - 1, (START_X - 1), "POINTS");
    for (int i = 0; i < lives; i++)
        mvwprintw(*win, LIVES_Y + i, UI_CENTER_X - (i % 2), "%s", SPRITE_FROG);
    for (int i = SIZE_FSCORE; i >= 0; i--)
        mvwprintw(*win, SCORE_Y + i, UI_CENTER_X + (i % 2), "%c\n", fscore[i]);
}
