#ifndef COMMON_HEADER
#define COMMON_HEADER

#define GSIZE 56 //16 fiume, margini 2*2, tane 6, 16x16 * 3.5
#define PSIZE 56/2
#define UISIZE 8

#define SPRITE_CURSOR '>'
#define SPRITE_FROG ">M<"

#define NLANES 8

enum Keys {
  Key_exit = 'Q',
  Key_quit = 'q',
  Key_pause = 'p',
  Key_up = 'w',
  Key_down = 's',
  Key_left = 'a',
  Key_right = 'd'
};

typedef enum GameStatus {
  Exit,
  Game,
  Menu
} gstate;

typedef enum {
  Id_frog,
  Id_croc_slow,
  Id_croc_normal,
  Id_croc_fast,
  Id_quit 
} msgid;

enum Speeds {
  Slow,
  Normal, 
  Fast
};

typedef struct Position {
  int x;
  int y;
} pos;

typedef struct Message {
  msgid id; 
  pos p;
  pos crocs[NLANES*8];
} msg;

#endif
