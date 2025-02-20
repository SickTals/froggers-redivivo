#ifndef MENU
#define MENU

#include <curses.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "ui.h"

#define MENU_START_Y 4
#define STATIC_SPACE 5
#define MSG_TO_STRING(msg) \
    ((msg) == Msg_play ? "PLAY"    : \
     (msg) == Msg_opts ? "OPTIONS" : \
     (msg) == Msg_quit ? "QUIT"    : "Unknown")

enum MenuMsg {
    Msg_play,
    Msg_opts,
    Msg_quit,
    Msg_all
};

void initMenu(WINDOW **win);
void menuPrintSelector(WINDOW **win, int sel);
gstate menu(WINDOW **g_win, WINDOW **ui_win);
void printMenu(WINDOW **win, int cursor);
gstate handleMenu(WINDOW **win, int *cursor);
gstate handleSelection(int cursor);
void PauseMenu(WINDOW **g_win);
void printPauseMenu(WINDOW **win, char sprite[5][33]);

#endif
