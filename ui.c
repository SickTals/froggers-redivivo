#include "ui.h"
#include "common.h"
#include "curses.h"

void printMenuUi(WINDOW **win)
{

    char sprite[35][7] = {
        "| )",
        "|\\",
        " __",
        "|__",
        "|__",
        "   ",
        "|~ ",
        "~| ",
        "   ",
        "| |",
        "|_|",
        "   ",
        "| )",
        "|\\",
        "   ",
        "| )",
        "|\\",
        " __",
        "|__",
        "|__",
        " __",
        "|  ",
        "|__",
        "___ ",
        " T ",
        " | ",
        "   ",
        " | ",
        " | ",
        " _ ",
        "| |",
        "|_|",
        "   ",
        "|\\ |",
        "| \\|"
    };

    wclear(*win);
    box(*win, ACS_VLINE, ACS_HLINE);
    for(int i = 0; i < 35; i++)
        mvwprintw(*win, 1, START_X, "%s\n", sprite[i]); // Update row to 4 + i
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
    mvwprintw(*win, TIMER_Y - 1, START_X + 1, "TIME");
    mvwprintw(*win, TIMER_Y, CENTER_X, "%d", t.objs[Id_timer].y);
    mvwprintw(*win, LIVES_Y - 1, START_X + 1, "LIVE");
    for (int i = 0; i < lives; i++)
        mvwprintw(*win, LIVES_Y + i, CENTER_X - (i % 2), "%s", SPRITE_FROG);
    mvwprintw(*win, SCORE_Y - 1, START_X, "POINTS");
    for (int i = SIZE_FSCORE; i >= 0; i--)
        mvwprintw(*win, SCORE_Y + i, CENTER_X + (i % 2), "%c\n", fscore[i]);
}
