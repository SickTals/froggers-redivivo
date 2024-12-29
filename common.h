#ifndef COMMON_HEADER
#define COMMON_HEADER

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
  ID_QUIT = 0,
  ID_FROG = 1
} msgid;

typedef struct Message {
  msgid id; 
  int y;
  int x;

} msg;

#endif
