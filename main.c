#include "main.h"
#include "common.h"
#include "curses.h"
#include "river.h"

int main() 
{
  gstate flag = Menu;
  WINDOW *g_win;
  WINDOW *ui_win;

  init_screen(&g_win, &ui_win);

  while (true) {
    if (flag == Menu)
      flag = menu(&g_win);
    if (flag == Exit)
      break;
    if (flag == Game)
      flag = game(&g_win);
  }

  end_screen(&g_win, &ui_win);
  return 0;
}

/*
 * Inizializza e imposta la schermata
 * Le finestre vengono posizionate centrate
 */
void init_screen(WINDOW **g_win, WINDOW **ui_win)
{
  initscr();
  noecho();
  curs_set(false);
  cbreak();
  *g_win = newwin(GSIZE/2, GSIZE,
                  (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) - UISIZE/2);
  *ui_win = newwin(GSIZE/2, UISIZE,
                   (LINES - GSIZE/2)/2, ((COLS - GSIZE)/2) + GSIZE - UISIZE/2);
  nodelay(*g_win, false);
  keypad(*g_win, true);
  box(*g_win, ACS_VLINE, ACS_HLINE);
  box(*ui_win, ACS_VLINE, ACS_HLINE);
  wrefresh(*g_win);
  wrefresh(*ui_win);
}

void end_screen(WINDOW **g_win, WINDOW **ui_win)
{
  delwin(*g_win);
  delwin(*ui_win);
  endwin();
}


int game(WINDOW **g_win)
{
  WINDOW *p_win;
  gstate flag = Game;
  int pipefd[2];
  pid_t pids[NTASKS];
  rvr river = generateRiver();

  if (pipe(pipefd) == -1) {
    perror("Pipe call");
    exit(1);
  }
  srand(time(NULL));

  p_win = newwin(PSIZE/2, PSIZE, (LINES - PSIZE/2)/2, (COLS - PSIZE)/2);
  box(p_win, ACS_VLINE, ACS_HLINE);

  for (int i = 0; i < NTASKS; i++) {
    pids[i] = fork();
    if (pids[i] < 0) {
      perror("Fork fail");
      exit(EXIT_FAILURE);
    } else if (pids[i] == 0) {
      switch (i) {
        case Id_frog:
          frog(g_win, pipefd);
          exit(0);
          break;
        case Id_croc_slow:
          crocodile(river, Slow, pipefd);
          exit(0);
          break;
        case Id_croc_normal:
          crocodile(river, Normal, pipefd);
          exit(0);
          break;
        case Id_croc_fast:
          crocodile(river, Fast, pipefd);
          exit(0);
          break;
      }
    }
  }
  
  msg msgs[NTASKS + 1];
  msgs[Id_frog].id = Id_frog;
  msgs[Id_frog].p.y = GSIZE/2 - 2;
  msgs[Id_frog].p.x = GSIZE/2;
  for (int i = 0; i < NLANES*8; i++) {
    msgs[Id_croc_slow].crocs[i].y = -10;
    msgs[Id_croc_slow].crocs[i].x = -10;
    msgs[Id_croc_normal].crocs[i].y = -10;
    msgs[Id_croc_normal].crocs[i].x = -10;
    msgs[Id_croc_fast].crocs[i].y = -10;
    msgs[Id_croc_fast].crocs[i].x = -10;
  }
  while (flag == Game) {
    close(pipefd[1]);
    
    wclear(*g_win);
    printCrocs(g_win, msgs[Id_croc_slow].crocs,
               msgs[Id_croc_normal].crocs, msgs[Id_croc_fast].crocs);
    printFrog(g_win, msgs[Id_frog]);
    box(*g_win, ACS_VLINE, ACS_HLINE);
    wrefresh(*g_win);
    
    (void)read(pipefd[0], &msgs[NTASKS], sizeof(msgs[NTASKS]));

    switch (msgs[NTASKS].id) {
      case Id_frog:
        msgs[Id_frog] = handleFrog(msgs[NTASKS].p, msgs[Id_frog]);
        break;
      case Id_quit:
        flag = Menu;
        break;
      case Id_croc_slow:
        msgs[Id_croc_slow] = handleCroc(msgs[NTASKS].crocs, msgs[Id_croc_slow]);
        break;
      case Id_croc_normal:
        msgs[Id_croc_normal] = handleCroc(msgs[NTASKS].crocs, msgs[Id_croc_normal]);
        break;
      case Id_croc_fast:
        msgs[Id_croc_fast] = handleCroc(msgs[NTASKS].crocs, msgs[Id_croc_fast]);
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

  delwin(p_win);
  return flag;
}

