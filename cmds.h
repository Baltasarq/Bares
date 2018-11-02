/* Commands */

#ifndef _INCLUDE_CMDS_H
#define _INCLUDE_CMDS_H

#define LENGTH_WORD 5
#define NUM_CMDS 20

#include "objs.h"

typedef enum _CmdId {
	Cmd_North, Cmd_South, Cmd_East, Cmd_West, Cmd_Up, Cmd_Down,
	Cmd_End,
	Cmd_Help,
	Cmd_Inventory,
	Cmd_Examine,
	Cmd_Exits,
	Cmd_Take,
	Cmd_Drop,
	Cmd_Swim,
	Cmd_Wear,
	Cmd_Disrobe,
    Cmd_Wait,
    Cmd_Break,
    Cmd_Push,

	Cmd_Nop
} CmdId;

typedef struct _Cmd {
	CmdId cmdId;
	char *words;
	void (*doIt)();
} Cmd;

typedef struct _Order {
	char * verb;
	char * word1;
	char * word2;

	Cmd  * cmd;
	Obj * obj1;
	Obj * obj2;
} Order;

extern Cmd cmds[];
extern Cmd * cmdNop;
extern void init_cmds();

#endif

