#ifndef COMMON
#define COMMON

#define GSIZE 56 //16 fiume, margini 2*2, tane 6, 16x16 * 3.5
#define PSIZE 56/2
#define UISIZE 8
#define UDELAY 500000

#define SPRITE_CURSOR '>'
#define SPRITE_FROG ">M<"

#define NLANES 8
#define CROC_CAP NLANES*NLANES

enum Keys {
    Key_exit = 'Q',
    Key_quit = 'q',
    Key_pause = 'p',
    Key_up = 'w',
    Key_down = 's',
    Key_left = 'a',
    Key_right = 'd',
    Key_shoot = ' '
};

typedef enum GameStatus {
    Exit,
    Game,
    Menu,
    Dies
} gstate;

typedef enum {
    Id_frog,
    Id_granade,
    Id_croc_slow,
    Id_croc_normal,
    Id_croc_fast,
    Id_quit 
} msgid;

enum Speeds {
    Slow = 2,
    Normal = 3, 
    Fast = 4
};

typedef struct Position {
    int x;
    int y;
} pos;

typedef struct Object {
    int x;
    int y;
    bool shoots;
} obj;

typedef struct Message {
    msgid id; 
    pos p;
    obj crocs[CROC_CAP];
    bool shoots;
    int sx_x;
} msg;

#endif
