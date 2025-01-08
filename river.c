#include "river.h"
#include "common.h"

rvr generateRiver()
{
  rvr r;
  r.firstDir = rand() % 2;
  for(int i = 0; i < NLANES; i++) {
    r.speeds[i] = rand() % NSPEEDS;
  }
  return r;
}

void crocodile(rvr river, int speed, int pipefd[])
{
  close(pipefd[0]);
  int free_counter[NLANES];
  int ncrocs = 0;
  msg msg_croc;
  switch (speed) {
    case Slow:
      msg_croc.id = Id_croc_slow;
      break;
    case Normal:
      msg_croc.id = Id_croc_normal;
      break;
    case Fast:
      msg_croc.id = Id_croc_fast;
      break;
  }
  for (int i = 0; i < NLANES*4; i++) {
    msg_croc.crocs[i].y = -1;
    msg_croc.crocs[i].x = -1;
  }
  for (int i = 0; i < NLANES; i++) {
    free_counter[i] = 11;
  }
  while (true) {
    for (int i = 0; i < NLANES; i++) {
      if (river.speeds[i] != speed)
        continue;
      if (free_counter[i] > 3*strlen(SPRITE_FROG) && 0 == rand() % 200/free_counter[i]) {
        ncrocs++;
        msg_croc.crocs[ncrocs-1].y = i;
        msg_croc.crocs[ncrocs-1].x = 0;
        free_counter[i] = 0;
      }
    }
    for (int i = 0; i < ncrocs; i++) {
      msg_croc.crocs[i].x += 1;
      free_counter[msg_croc.crocs[i].y] += 1;
      if (msg_croc.crocs[i].x >= GSIZE) {
        msg_croc.crocs[i].y = -1;
        msg_croc.crocs[i].x = -1;
        swapPos(msg_croc.crocs, i, ncrocs-1);
        ncrocs--;
      }
    }
    (void)write(pipefd[1], &msg_croc, sizeof(msg_croc));
    switch (speed) {
      case Slow:
        usleep(UDELAY*2);
        break;
      case Normal:
        usleep(UDELAY);
        break;
      case Fast:
        usleep(UDELAY/2);
        break;
    }
  }
  close(pipefd[1]);
}

void swapPos(pos a[], int i, int j)
{
  pos tmp = a[i];
  a[i] = a[j];
  a[j] = tmp;
}

msg handleCroc(pos p[], msg c) {
  for (int i = 0; i < NLANES*4; i++) {
    if (c.p.x < 0)
      break;
    c.crocs[i] = p[i];
  }
  return c;
}
