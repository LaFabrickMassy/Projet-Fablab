#ifndef _EXPLORE_H_
#define _EXPLORE_H

#define EX_STATE_STOP 0
#define EX_STATE_RUN 1
#define EX_STATE_RUN_END 2
#define EX_STATE_ROTATE 3
#define EX_STATE_ROTATE_END 4
#define EX_STATE_TURN 6
#define EX_STATE_TURN_END 7
#define EX_STATE_CRASH 99

void exploreInit();
void exploreStop();
void exploreStep();
void exploreRunStep();
void exploreHandleMessage(char *message);

#endif
