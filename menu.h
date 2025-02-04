#ifndef MENU
#define MENU

#include <curses.h>
#include <string.h>
#include "common.h"

enum MenuMsg {
  Msg_play,
  Msg_opts,
  Msg_quit
};

#define MSG_TO_STRING(msg) \
    ((msg) == Msg_play ? "PLAY"    : \
     (msg) == Msg_opts ? "OPTIONS" : \
     (msg) == Msg_quit ? "QUIT"    : "Unknown")

#define MSG_TO_STRING_P(msg) \
    ((msg) == Msg_play ? "RESUME" : \
     (msg) == Msg_quit ? "QUIT" : "Unknown")

gstate menu(WINDOW **g_win);
void printMenu(WINDOW **win, int cursor);
gstate handleMenu(WINDOW **win, int *cursor);
gstate handleSelection(int cursor);
gstate PauseMenu(WINDOW **g_win);
void printPauseMenu(WINDOW **win, int cursor);
gstate handlePauseMenu(WINDOW **win, int *cursor);
gstate handlePauseSelection(int cursor);
#endif
