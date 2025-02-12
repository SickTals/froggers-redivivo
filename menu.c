#include "menu.h"
#include "common.h"
#include "curses.h"


/* 
 * Stampa il menu
 * Restituisce
 *  false - Esce dal gioco
 *  true - Avvia gioco
 */
gstate menu(WINDOW **g_win, WINDOW **ui_win)
{
    int cursor = 0;
    gstate flag = Menu;

    while(flag == Menu) {
        printMenuUi(ui_win);
        printMenu(g_win, cursor);
        flag = handleMenu(g_win, &cursor);
    }
    return flag;
}

void printMenu(WINDOW **win, int cursor)
{
    char froggers_sprite[6][43] = {" _____                                    ",
                                   "|  ___| __ ___   __ _  __ _  ___ _ __ ___ ",
                                   "| |_ | '__/ _ \\ / _` |/ _` |/ _ \\ '__/ __|",
                                   "|  _|| | | (_) | (_| | (_| |  __/ |  \\__ \\",
                                   "|_|  |_|  \\___/ \\__, |\\__, |\\___|_|  |___/",
                                   "                |___/ |___/               "};
    wclear(*win);
    box(*win, ACS_VLINE, ACS_HLINE);

    for (int x = (GSIZE/8)*3 + 1; x < (GSIZE/8)*5 - 1; x++) {
        mvwprintw(*win, ((GSIZE - 6)/5) + MENU_START_Y/2 , x, "%c" , '_');  // Top border
        mvwprintw(*win, ((GSIZE/8)*3), x, "%c" , '_');  // Bottom border
    }

    // Left and right borders
    for (int y = ((GSIZE - 6)/5) + 3; y <= ((GSIZE/8)*3) ; y++) {
        mvwprintw(*win, y, (GSIZE/8)*3 , "%c" , '|');  // Left border
        mvwprintw(*win, y, (GSIZE/8)*5 - 1 , "%c" ,'|');  // Right border
    }


    for(int i = 0; i < 6; i++)
        mvwprintw(*win, MENU_START_Y + i, (GSIZE - PSIZE)/2, "%s", froggers_sprite[i]);

    mvwprintw(*win, GSIZE/4 , GSIZE/2 - strlen(MSG_TO_STRING(Msg_play))/2,
              MSG_TO_STRING(Msg_play));
    mvwprintw(*win, (GSIZE - 6)/5 + (STATIC_SPACE * 3)/2 , GSIZE/2 - strlen(MSG_TO_STRING(Msg_opts))/2,
              MSG_TO_STRING(Msg_opts));
    mvwprintw(*win, (GSIZE - 6)/5 + STATIC_SPACE * 2 , GSIZE/2 - strlen(MSG_TO_STRING(Msg_quit))/2,
              MSG_TO_STRING(Msg_quit));

    switch (cursor) {
        case Msg_play:
            mvwaddch(*win, GSIZE/4, GSIZE/2 - strlen(MSG_TO_STRING(Msg_play))/2 - 2,
                     SPRITE_CURSOR);
            break;
        case Msg_opts:
            mvwaddch(*win, (GSIZE - 6)/5 + (STATIC_SPACE * 3)/2 , GSIZE/2 - strlen(MSG_TO_STRING(Msg_opts))/2 - 2,
                     SPRITE_CURSOR);
            break;
        case Msg_quit:
            mvwaddch(*win, (GSIZE - 6)/5 + STATIC_SPACE * 2 , GSIZE/2 - strlen(MSG_TO_STRING(Msg_quit))/2 - 2,
                     SPRITE_CURSOR);
            break;
    }
    wrefresh(*win);
}

gstate handleSelection(int cursor)
{
  switch(cursor) {
    case 0:
      return Game;
    case 1:
        // TODO: for now just exit
    case 2:
      return Exit;
    default:
      return Exit;
  }
}

gstate handleMenu(WINDOW **win, int *cursor)
{
  char user_input = wgetch(*win);
  switch (user_input) {
    case Key_quit:
      return Exit;
    case (char)KEY_UP:
    case Key_up:
      *cursor -= 1;
      break;
    case (char)KEY_DOWN:
    case Key_down:
      *cursor += 1;
      break;
    case '\n':
    case ' ':
      return handleSelection(*cursor);
  }

  if (*cursor < Msg_play)
    *cursor = Msg_quit;
  if (*cursor > Msg_quit)
    *cursor = Msg_play;

  return Menu;
}

void PauseMenu(WINDOW **p_win)
{
    gstate flag = Pmenu;
    char sprite[5][33] = {
            "  ____                          ",
            " |  _ \\ __ _ _   _ ___  ___    ",
            " | |_) / _` | | | / __|/ _ \\   ",
            " |  __/ (_| | |_| \\__ \\  __/  ",
            " |_|   \\__,_|\\__,_|___/\\___| "
    };

    while(flag == Pmenu){
        wclear(*p_win);
        printPauseMenu(p_win, sprite);
        wrefresh(*p_win);
        char user_input = wgetch(*p_win);
        switch (user_input) {
            case Key_pause:
            case '\n':
            case ' ':
                flag = Game;
                break;
            default:
                flag = Pmenu;
                break;
        }
    }
}

void printPauseMenu(WINDOW **win, char sprite[5][33])
{
    box(*win, ACS_VLINE, ACS_HLINE);
    for(int i = 0; i < 5; i++)
        mvwprintw(*win, MENU_START_Y + i, GSIZE/8, "%s", sprite[i]); // Update row to 4 + i
    wrefresh(*win); // Ensure the window is refreshed
}
