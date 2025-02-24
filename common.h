#ifndef COMMON
#define COMMON

#define GSIZE 56 //heigh GSIZE/2: 1 margine alto, 2-8 prato tane, 9-25 fiume a 2 a 2, 26-27 pratino rana
#define PSIZE 42
#define UISIZE 8
#define UDELAY 500000

#define COLOR_BROWN 70

#define NLANES 8
#define CROC_CAP NLANES*NLANES
#define SIZE_PIXEL 2

#define CALC_LANE(y) (GSIZE/2 - 2 - y - 1) / 2
#define CALC_DIRECTION(isRight, lane) (isRight + lane) % 2 == 0;

#define SPRITE_CURSOR '>'
#define SPRITE_LIFE ">M<"
#define SPRITE_FROG \
    "<M>",          \
    "/W\\"
#define SPRITE_GREN \
    '\\','/'
#define SPRITE_CROC_LEFT \
    "}[][][]{<",    \
    " M   M   "
#define SPRITE_CROC_RIGHT \
    ">}[][][]{", \
    "  M   M  "
#define SPRITE_PROJ \
    '%',            \
    '#'
#define SPRITE_DEN_OPEN \
    "\\/][\\/",         \
    "| () |",           \
    " \\__/ "

#define SPRITE_DEN_CLOSE \
    "\\/__\\/",          \
    "|=[]=|",            \
    " \\][/ "

#define SPRITE_MTITLE \
" _____                                    ",\
"|  ___| __ ___   __ _  __ _  ___ _ __ ___ ",\
"| |_ | '__/ _ \\ / _` |/ _` |/ _ \\ '__/ __|",\
"|  _|| | | (_) | (_| | (_| |  __/ |  \\__ \\",\
"|_|  |_|  \\___/ \\__, |\\__, |\\___|_|  |___/",\
"                |___/ |___/               "

#define SPRITE_STITLE \
"[*)",\
"|\\_",\
"[__",\
"[__",\
"/_",\
" _/",\
"| |",\
"|_|",\
"[*)",\
"|\\ ",\
"[*)",\
"|\\_",\
"[__",\
"[__",\
"/  ",\
"\\__",\
"___",\
" | ",\
" ' ",\
" | ",\
"/ \\",\
"\\_/",\
"|\\|",\
"| \\"

#define SPRITE_PAUSE \
"  ____                          ",\
" |  _ \\ __ _ _   _ ___  ___    ",\
" | |_) / _` | | | / __|/ _ \\   ",\
" |  __/ (_| | |_| \\__ \\  __/  ",\
" |_|   \\__,_|\\__,_|___/\\___| "

#define SPRITE_WIN \
"__   __           __        __ _        ",\
"\\ \\ / /___   _   _\\ \\      / /(_) _ __  ",\
" \\ V // _ \\ | | | |\\ \\ /\\ / / | || '_ \\ ",\
"  | || (_) || |_| | \\ V  V /  | || | | |",\
"  |_| \\___/  \\__,_|  \\_/\\_/   |_||_| |_|"

#define SPRITE_LOSE \
"__   __             _                      ",\
"\\ \\ / /___   _   _ | |     ___   ___   ___ ",\
" \\ V // _ \\ | | | || |    / _ \\ / __| / _ \\",\
"  | || (_) || |_| || |___| (_) |\\__ \\|  __/",\
"  |_| \\___/  \\__,_||_____|\\___/ |___/ \\___|"

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

enum Colors_sprite{
    Grass = 1,
    Frog,
    Crocs,
    River,
    Dens,
    Proj,
    Gren,
    Ui,
    Lives,
    Evil_Ui,
    Alt_E_Ui,
    Sprite_Ui
};

typedef enum GameStatus {
    Exit,
    Game,
    Menu,
    Dies,
    Win,
    Pmenu,
    EndW,
    EndL,
    Options,
    SprOpt
} gstate;

typedef enum {
    Id_frog,
    Id_granade,
    Id_croc_slow,
    Id_croc_normal,
    Id_croc_fast,
    Id_croc_projectile,
    Id_timer,
    Id_quit,
    Id_pause
} msgid;

enum Speeds {
    Slow = Id_croc_slow,
    Normal = Id_croc_normal, 
    Fast = Id_croc_fast
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
    obj objs[CROC_CAP];
} msg;

#endif
