#ifndef _MICROMOUSE_H_

#define _MICROMOUSE_H_

// modes
#define ROBOT_MODE_STOP 0
#define ROBOT_MODE_TEST_DRIVE 1
#define ROBOT_MODE_PARAM 2
// States
#define ROBOT_STATE_STOP 0
#define ROBOT_STATE_RUN 1
#define ROBOT_STATE_RUN_END 2
#define ROBOT_STATE_ROTATE 3
#define ROBOT_STATE_ROTATE_END 4
#define ROBOT_STATE_TURN 5
#define ROBOT_STATE_TURN_END 6
#define ROBOT_STATE_CRASH 99

extern int current_mode;
extern int current_state;

extern motor_t motorL; // left motor
extern motor_t motorR; // right motor

extern const char* ssid;
extern const char* password;

extern double speed;
extern double turn;
extern double pos_x;
extern double pos_y;
extern double heading;
extern double mov_x, mov_y;
extern double mov_h; 

extern String debug_message;

#endif