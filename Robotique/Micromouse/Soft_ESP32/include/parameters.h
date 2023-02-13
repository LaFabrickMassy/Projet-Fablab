

#ifndef PARAMETERS_H
#define PARAMETERS_H

#define NB_OF_SENSORS 3

// Wifi mode
#define WIFI_MODE_AP 1

// modes
#define ROBOT_MODE_STOP 0
#define ROBOT_MODE_SIMPLE_RUN 1
#define ROBOT_MODE_PID_CAL 2
// States
#define ROBOT_STATE_STOP 0
#define ROBOT_STATE_SR_RUN 1
#define ROBOT_STATE_PID_CAL_RUN 2
#define ROBOT_STATE_PID_CAL_END 3
#define ROBOT_STATE_PID_CAL_TURN 4

// Simple run parameters
#define SPEED_MIN 0.1
#define SPEED_FACTOR 1.4
#define TURN_STEP 0.1

// PID initial parameters
#define PID_INITIAL_KP 2.2
#define PID_INITIAL_KI 0
#define PID_INITIAL_KD 0
#define PID_MIN 0.01
#define PID_STEP 1.2 // Step for up/down commands
#define PID_MAX_ERROR 50

// Maze parameters
#define MAZE_WALL_LENGTH 204
#define MAZE_WALL_THICKNESS 9
#define MAZE_X_SIZE 9
#define MAZE_Y_SIZE 9

// Wall minimal distance for crash detection
#define WALL_DISTANCE_FMIN 50 // front distance
#define WALL_DISTANCE_LMIN 30 // border distance

// Robot mechanics
#define WHEEL_DISTANCE 95.
#define WHEEL_CIRCUMFERENCE 100.531

//* Motors and encoders definitions ***********************
// DIR1 : yellow 4
// PWM1 : green 0
// PWM2 : Blue 2
// DIR2 : white 15
// S1B : grey 27
// S1A : purple 26
// S2A : orange 25
// S2B : brown 33
#define PIN_L_MOTOR_SPEED  0 // green
#define PIN_L_MOTOR_DIR    4 // yellow
#define PIN_L_MOTOR_SA    26 // purple
#define PIN_L_MOTOR_SB    27 // grey
#define PWMCH_L_MOTOR      0 // PWM channel

#define PIN_R_MOTOR_SPEED  2 // blue
#define PIN_R_MOTOR_DIR   15 // white
#define PIN_R_MOTOR_SA    25 // orange
#define PIN_R_MOTOR_SB    33 // brown
#define PWMCH_R_MOTOR      1 // PWM channel

//* Sensors definitions ***********************************
#define PIN_SENSORL 19
#define PIN_SENSORF 18
#define PIN_SENSORR 5

// Wheel encoders
#define TICKS_PER_TURN 3.
#define GEAR_RATIO 150.
#define MM_PER_TICK (WHEEL_CIRCUMFERENCE/(TICKS_PER_TURN*GEAR_RATIO))

#define PI 3.1415926535897932384626433832795


#endif