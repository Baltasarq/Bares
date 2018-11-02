/* Rooms */

#ifndef _INCLUDE_LOCS_H
#define _INCLUDE_LOCS_H

#define NUM_LOCS 24
#define NUM_EXITS 6

typedef enum _Exits {
    Exit_North,
    Exit_South,
    Exit_East,
    Exit_West,
    Exit_Up,
    Exit_Down
} Exits;

typedef struct _Loc {
  char * desc;
  int exits[NUM_EXITS];
} Loc;

extern Loc locs[];
extern void init_locs();

#endif
