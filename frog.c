#include "frog.h"

void frog(WINDOW **win, int pipefd[]){
  close(pipefd[0]);
  char user_input;
  msg msg_frog;
  msg_frog.id = Id_frog;
  while(true){ // Ciclo infinito chiuso dal padre
    msg_frog.p.y = 0;
    msg_frog.p.x = 0;
    user_input = wgetch(*win);
    flushinp();
    switch(user_input){
      case Key_up: 
          msg_frog.p.y = (int)'+';
          break;
        case Key_down:
          msg_frog.p.y = (int)'-';
          break;
        case Key_left:
          msg_frog.p.x = (int)'-';
          break;
        case Key_right:
          msg_frog.p.x = (int)'+';
          break;
    // TODO: funzione della granata e pausa
    //  case SHOOT_KEY: break;      
    //  case PAUSE_KEY: break;
        case Key_quit:
          msg_frog.id = Id_quit;
          break;
    }
    // Scrittura su pipe della posizione dell'oggetto
    (void)write(pipefd[1], &msg_frog, sizeof(msg_frog));
  }
  close(pipefd[1]);
}

msg handleFrog(pos p, msg f)
{
  if (p.y == '+')
    f.p.y--;
  else if (p.y == '-')
    f.p.y++;
  if (p.x == '-')
    f.p.x -= strlen(SPRITE_FROG);
  else if (p.x == '+')
    f.p.x += strlen(SPRITE_FROG);

  if (f.p.y < 0)
    f.p.y++;
  else if (f.p.y > GSIZE/2)
    f.p.y--;
  if (f.p.x < 0)
    f.p.x += strlen(SPRITE_FROG);
  else if (f.p.x > GSIZE)
    f.p.x -= strlen(SPRITE_FROG);

  return f;
}

void printFrog(WINDOW **g_win, msg f)
{
  mvwprintw(*g_win, f.p.y, f.p.x, SPRITE_FROG);
}
