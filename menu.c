#include "menu.h"
/* 
 * Stampa il menu
 * Restituisce
 *  false - Esce dal gioco
 *  true - Avvia gioco
 */
gstate menu(WINDOW **g_win)
{
  int cursor = 0;
  int flag = 2;

  while(flag == 2) {
    wclear(*g_win);
    printMenu(g_win, cursor);
    wrefresh(*g_win);
    flag = manageMenu(g_win, &cursor);
  }
  return flag;
}

void printMenu(WINDOW **win, int cursor)
{
  box(*win, ACS_VLINE, ACS_HLINE);
  mvwprintw(*win, GSIZE/8, GSIZE/2 - strlen(PLAY_MSG)/2, PLAY_MSG);
  mvwprintw(*win, GSIZE/4, GSIZE/2 - strlen(OPTIONS_MSG)/2, OPTIONS_MSG);
  mvwprintw(*win, GSIZE/4 + GSIZE/8, GSIZE/2 - strlen(QUIT_MSG)/2, QUIT_MSG);
  switch (cursor) {
    case 0:
      mvwaddch(*win, GSIZE/8, GSIZE/2 - strlen(PLAY_MSG)/2 - 2, SPRITE_CURSOR);
      break;
    case 1:
      mvwaddch(*win, GSIZE/4, GSIZE/2 - strlen(OPTIONS_MSG)/2 - 2, SPRITE_CURSOR);
      break;
    case 2:
      mvwaddch(*win, GSIZE/4 + GSIZE/8, GSIZE/2 - strlen(QUIT_MSG)/2 - 2, SPRITE_CURSOR);
      break;
  }
}

gstate manageMenu(WINDOW **win, int *cursor)
{
  char user_input = wgetch(*win);
  switch (user_input) {
    case QUIT_KEY:
      return 0;
    case (char)KEY_UP:
    case UP_KEY:
      *cursor -= 1;
      break;
    case (char)KEY_DOWN:
    case DOWN_KEY:
      *cursor += 1;
      break;
    case '\n':
    case ' ':
      if (*cursor == 0)
        return 1;
      else if (*cursor == 2)
        return 0;
      break;
  }

  if (*cursor < 0)
    *cursor = 2;
  if (*cursor > 2)
    *cursor = 0;

  return 2;
}
