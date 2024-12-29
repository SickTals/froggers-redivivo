#include "main.h"
#include "common.h"
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
      flag = menu(&g_win);
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
  curs_set(false);
  cbreak();
  *g_win = newwin(GSIZE/2, GSIZE, (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) - UISIZE/2);
  *ui_win = newwin(GSIZE/2, UISIZE, (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) + GSIZE - UISIZE/2);
  *p_win = newwin(PSIZE/2, PSIZE, (LINES - PSIZE/2)/2, (COLS - PSIZE)/2);
  nodelay(*g_win, false);
  keypad(*g_win, true);
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
    
    (void)read(pipefd[0], &msgs[NTASKS], sizeof(msgs[NTASKS]));

    switch (msgs[NTASKS].id) {
      case ID_FROG:
        msgs[ID_FROG] = manageFrog(msgs[NTASKS].y, msgs[NTASKS].x, msgs[ID_FROG]);
        break;
      case ID_QUIT:
        flag = 2;
        break;
    }
  }
  close(pipefd[0]);
  for (int i = 0; i < NTASKS; i++) {
    if (kill(pids[i], SIGKILL) == -1) {
      perror("kill failed");
      return 1;
      wait(NULL);
    }
  }

  return flag;
}

msg manageFrog(int y, int x, msg f)
{
  if (y == '+')
    f.y--;
  else if (y == '-')
    f.y++;
  if (x == '-')
    f.x -= strlen(SPRITE_FROG);
  else if (x == '+')
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
