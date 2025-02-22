#ifndef MENU
#define MENU

#include <curses.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "ui.h"

#define MENU_START_Y 4
#define STATIC_SPACE 5
#define WIDTH_MTITLE 43
#define LENGHT_MTITLE 6
#define WIDTH_PAUSE 33
#define LENGHT_PAUSE 5
#define MSG_TO_STRING(msg) \
    ((msg) == Msg_play ? "PLAY"    : \
     (msg) == Msg_opts ? "OPTIONS" : \
     (msg) == Msg_quit ? "QUIT"    : "Unknown")

void ui_animation(int pipefd[], pid_t pid);
gstate menu(WINDOW **g_win, WINDOW **ui_win);
void printMenu(WINDOW **win, int cursor);
gstate handleMenu(WINDOW **win, int *cursor);
gstate handleSelection(int cursor);
gstate optionsMenu(WINDOW **g_win, WINDOW **ui_win);
void pauseMenu(WINDOW **win);
void printPauseMenu(WINDOW **win);

enum MenuMsg {
  Msg_play,
  Msg_opts,
  Msg_quit,
};

#endif
