#include "menu.h"
#include "common.h"
/* 
 * Stampa il menu
 * Restituisce
 *  false - Esce dal gioco
 *  true - Avvia gioco
 */
gstate menu(WINDOW **g_win)
{
  int cursor = 0;
  gstate flag = Menu;

  while(flag == Menu) {
    wclear(*g_win);
    printMenu(g_win, cursor);
    wrefresh(*g_win);
    flag = handleMenu(g_win, &cursor);
  }
  return flag;
}

void printMenu(WINDOW **win, int cursor)
{
  box(*win, ACS_VLINE, ACS_HLINE);
  mvwprintw(*win, GSIZE/8, GSIZE/2 - strlen(MSG_TO_STRING(Msg_play))/2,
            MSG_TO_STRING(Msg_play));
  mvwprintw(*win, GSIZE/4, GSIZE/2 - strlen(MSG_TO_STRING(Msg_opts))/2,
            MSG_TO_STRING(Msg_opts));
  mvwprintw(*win, GSIZE/4 + GSIZE/8, GSIZE/2 - strlen(MSG_TO_STRING(Msg_quit))/2,
            MSG_TO_STRING(Msg_quit));
  switch (cursor) {
    case Msg_play:
      mvwaddch(*win, GSIZE/8, GSIZE/2 - strlen(MSG_TO_STRING(Msg_play))/2 - 2,
               SPRITE_CURSOR);
      break;
    case Msg_opts:
      mvwaddch(*win, GSIZE/4, GSIZE/2 - strlen(MSG_TO_STRING(Msg_opts))/2 - 2,
               SPRITE_CURSOR);
      break;
    case Msg_quit:
      mvwaddch(*win, GSIZE/4 + GSIZE/8, GSIZE/2 - strlen(MSG_TO_STRING(Msg_quit))/2 - 2,
               SPRITE_CURSOR);
      break;
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
