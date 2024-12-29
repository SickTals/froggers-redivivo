#include "main.h"
#include "curses.h"

int main() 
{
  int flag = 2;
  WINDOW *g_win;
  WINDOW *ui_win;
  WINDOW *p_win;

  init_screen(&g_win, &ui_win, &p_win);

  while (true) {
    if (flag == 2)
      flag = menu(&g_win, flag);
    if (flag == 0)
      break;
    if (flag == 1)
      flag = game(&g_win);
  }

  end_screen(&g_win, &ui_win, &p_win);
  return 0;
}

/*
 * Inizializza e imposta la schermata
 * Le finestre vengono posizionate centrate
 */
void init_screen(WINDOW **g_win, WINDOW **ui_win, WINDOW **p_win)
{
  initscr();
  noecho();
  curs_set(0);
  cbreak();
  *g_win = newwin(GSIZE/2, GSIZE, (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) - UISIZE/2);
  *ui_win = newwin(GSIZE/2, UISIZE, (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) + GSIZE - UISIZE/2);
  *p_win = newwin(PSIZE/2, PSIZE, (LINES - PSIZE/2)/2, (COLS - PSIZE)/2);
  nodelay(*g_win, 0);
  box(*g_win, ACS_VLINE, ACS_HLINE);
  box(*ui_win, ACS_VLINE, ACS_HLINE);
  box(*p_win, ACS_VLINE, ACS_HLINE);
  wrefresh(*g_win);
  wrefresh(*ui_win);
}

void end_screen(WINDOW **g_win, WINDOW **ui_win, WINDOW **p_win)
{
  delwin(*g_win);
  delwin(*ui_win);
  delwin(*p_win);
  endwin();
}

/* 
 * Stampa il menu
 * Restituisce
 *  false - Esce dal gioco
 *  true - Avvia gioco
 */
bool menu(WINDOW **g_win, int flag)
{
  int cursor = 0;
  int user_input = -1;

  while(flag == 2) {
    wclear(*g_win);
    box(*g_win, ACS_VLINE, ACS_HLINE);
    mvwprintw(*g_win, GSIZE/8, GSIZE/2 - strlen(PLAY_MSG)/2, "%s", PLAY_MSG);
    mvwprintw(*g_win, GSIZE/4, GSIZE/2 - strlen(OPTIONS_MSG)/2, "%s", OPTIONS_MSG);
    mvwprintw(*g_win, GSIZE/4 + GSIZE/8, GSIZE/2 - strlen(QUIT_MSG)/2, "%s", QUIT_MSG);
    switch (cursor) {
      case 0:
        mvwprintw(*g_win, GSIZE/8, GSIZE/2 - strlen(PLAY_MSG)/2 - 2, "%c",  SPRITE_CURSOR);
        break;
      case 1:
        mvwprintw(*g_win, GSIZE/4, GSIZE/2 - strlen(OPTIONS_MSG)/2 - 2, "%c", SPRITE_CURSOR);
        break;
      case 2:
        mvwprintw(*g_win, GSIZE/4 + GSIZE/8, GSIZE/2 - strlen(QUIT_MSG)/2 - 2, "%c", SPRITE_CURSOR);
        break;
    }
    wrefresh(*g_win);
    user_input = wgetch(*g_win);
    switch (user_input) {
      case 'k':
        cursor--; break;
      case 'j':
        cursor++; break;
      case '\n':
        if (cursor == 0)
          flag = 1;
        else if (cursor == 2)
          flag = 0;
        break;
      default:
        break;
    }
    if (cursor < 0)
      cursor = 2;
    if (cursor > 2)
      cursor = 0;
  }
  return flag;
}

int game(WINDOW **g_win)
{
  int flag = 1;
  int pipefd[2];
  pid_t pids[NTASKS];

  if (pipe(pipefd) == -1) {
    perror("Pipe call");
    exit(1);
  }
  srand(time(NULL));

  for (int i = 0; i < NTASKS; i++) {
    pids[i] = fork();
    if (pids[i] < 0) {
      perror("Fork fail");
      exit(EXIT_FAILURE);
    } else if (pids[i] == 0) {
      switch (i) {
        case 0:
          frog(g_win, pipefd);
          exit(0);
          break;
        //case 1:
          //break;
        //case 2:
          //break;
      }
    }
  }
  
  msg msgs[NTASKS + 1];
  msgs[ID_FROG].id = ID_FROG;
  msgs[ID_FROG].y = 2;
  msgs[ID_FROG].x = 2;
  while (flag == 1) {
    close(pipefd[1]);
    
    wclear(*g_win);
    box(*g_win, ACS_VLINE, ACS_HLINE);
    printFrog(g_win, msgs[ID_FROG]);
    wrefresh(*g_win);
    
    (void)read(pipefd[0], &msgs[0], sizeof(msgs[0]));

    switch (msgs[0].id) {
      case ID_QUIT:
        flag = 2;
        break;
      case ID_FROG:
        msgs[ID_FROG] = manageFrog(msgs[0].y, msgs[0].x, msgs[ID_FROG]);
        break;
    }
  }
  close(pipefd[0]);
  for (int i = 0; i < NTASKS; i++) {
    if (kill(pids[0], SIGKILL) == -1) {
      perror("kill failed");
      return 1;
      wait(NULL);
    }
  }

  return flag;
}

msg manageFrog(int y, int x, msg f)
{
  if (y == (int)'+')
    f.y--;
  else if (y == (int)'-')
    f.y++;
  if (x == (int)'-')
    f.x -= strlen(SPRITE_FROG);
  else if (x == (int)'+')
    f.x += strlen(SPRITE_FROG);

  if (f.y < 0)
    f.y++;
  else if (f.y > GSIZE/2)
    f.y--;
  if (f.x < 0)
    f.x += strlen(SPRITE_FROG);
  else if (f.x > GSIZE)
    f.x -= strlen(SPRITE_FROG);

  return f;
}

void printFrog(WINDOW **g_win, msg f)
{
  mvwprintw(*g_win, f.y, f.x, SPRITE_FROG);
}
