#ifndef COMMON_HEADER
#define COMMON_HEADER

#define GSIZE 56 //36 fiume, 2*10 margini, 16x16 * 3.5
#define PSIZE 56/2
#define UISIZE 8

#define SPRITE_CURSOR '>'
#define SPRITE_FROG ">M<"

typedef enum GameStatus {
  EXIT,
  GAME,
  MENU
} gstate;

enum Keys {
  EXIT_KEY = 'Q',
  QUIT_KEY = 'q',
  PAUSE_KEY = 'p',
  UP_KEY = 'w',
  DOWN_KEY = 's',
  LEFT_KEY = 'a',
  RIGHT_KEY = 'd'
};

typedef enum {
  ID_FROG = 0,
  ID_QUIT 
} msgid;

typedef struct Message {
  msgid id; 
  int y;
  int x;

} msg;

#endif
