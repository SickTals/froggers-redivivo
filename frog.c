#include "frog.h"

void frog(WINDOW **win, int pipefd[]){
  close(pipefd[0]);
  char user_input;
  msg msg_frog;
  msg_frog.id = ID_FROG;
  while(true){ // Ciclo infinito chiuso dal padre
    msg_frog.y = 0;
    msg_frog.x = 0;
    user_input = wgetch(*win);
    flushinp();
    switch(user_input){
      case UP_KEY: 
          msg_frog.y = (int)'+';
          break;
        case DOWN_KEY:
          msg_frog.y = (int)'-';
          break;
        case LEFT_KEY:
          msg_frog.x = (int)'-';
          break;
        case RIGHT_KEY:
          msg_frog.x = (int)'+';
          break;
    // TODO: funzione della granata e pausa
    //  case SHOOT_KEY: break;      
    //  case PAUSE_KEY: break;
        case QUIT_KEY:
          msg_frog.id = ID_QUIT;
          break;
    }
    // Scrittura su pipe della posizione dell'oggetto
    (void)write(pipefd[1], &msg_frog, sizeof(msg_frog));
  }
  close(pipefd[1]);
}
