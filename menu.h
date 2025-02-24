#ifndef MENU
#define MENU

#include <curses.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "ui.h"

#define MENU_START_Y 4
#define WIDTH_MTITLE 43
#define LENGHT_MTITLE 6
#define WIDTH_PAUSE 33
#define LENGHT_PAUSE 5

#define BORDER_START_X (GSIZE/8)*3
#define BORDER_END_X (GSIZE/8)*5
#define BORDER_START_Y (((GSIZE - 6)/5) + MENU_START_Y/2)
#define STATIC_SPACE 5
#define MSG_TO_STRING(msg) \
    ((msg) == Msg_play ? "PLAY"    : \
     (msg) == Msg_opts ? "OPTIONS" : \
     (msg) == Msg_quit ? "QUIT"    : \
     (msg) == Msg_difficulty ? "MODE" : \
     (msg) == Msg_sprite     ? "SPRITE" : \
     (msg) == Msg_back       ? "BACK"       : \
     (msg) == Pair_one              ? " >M< " : \
     (msg) == Pair_two              ? " >M< " : \
     (msg) == Pair_three            ? " >M< " : "Unknown")
#define IS_OPTIONS(c) (c >= Msg_difficulty && c <= Msg_back)
#define IS_SPRITE_OPTIONS(c) (c >= Pair_one && c<= Pair_three)

gstate menu(WINDOW **g_win, WINDOW **ui_win, int cursor, gstate flag);
void printMenu(WINDOW **win, int cursor);
gstate handleMenu(WINDOW **win, int *cursor);
gstate handleSelection(int cursor);
gstate optionsMenu(WINDOW **g_win, WINDOW **ui_win);
void pauseMenu(WINDOW **win);
void printPauseMenu(WINDOW **win, const char *sprite[]);
gstate colorChangeFrog(int pair);

enum MenuMsg {
    Msg_play,
    Msg_opts,
    Msg_quit,
    Color_menu,
    Msg_difficulty,
    Msg_sprite,
    Msg_back,
    Color_options,
    Pair_one,
    Pair_two,
    Pair_three,
    Sprite_options
};

#endif
