

#ifndef _PARAMETERS_H
#define _PARAMETERS_H

//#define DEBUG 1
//#define DEBUG_MOVE 1

// Trace level:
// 1: IHM, state change
// 2: robot control init and stop
#define TRACE_LEVEL 2
#define LOG_SENSORPID_ERRORS 1

#define NB_OF_SENSORS 3

// Wifi mode
#define WIFI_MODE_AP 1

// Simple run parameters
#define SPEED_MIN 0.2 // Minimal speed, needed to start
#define SPEED_FACTOR 1.4
#define TURN_STEP 0.1

// Rotation parameters
#define SPEEDUP_PART 0.1

// PID parameters
#define PIDSENSORS_KP 0.022
#define PIDSENSORS_KI 0
#define PIDSENSORS_KD 0
#define PIDMOTORS_KP 0
#define PIDMOTORS_KI 0
#define PIDMOTORS_KD 0

// Maze parameters
#define MAZE_WALL_LENGTH 204
#define MAZE_WALL_THICKNESS 9
#define MAZE_X_SIZE 9
#define MAZE_Y_SIZE 9

// Wall minimal distance for crash detection
#define WALL_DISTANCE_FMIN 50 // front distance
#define WALL_DISTANCE_LMIN 30 // border distance

// Robot mechanics
#define WHEEL_DISTANCE 93.
// dist=-26832(76) - EncL=124678/-0.22mm/t - EncR=124407/-0.22mm/t - 
#define ENCL_RESOL 0.2152103819438874540817144965431
#define ENCR_RESOL 0.21567918203959584267766283247727
// Sensors stats
#define SENSORS_STDDEV 2.35 // Standard deviation of e=dL-dR

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
#define PIN_L_MOTOR_SA    33 // brown
#define PIN_L_MOTOR_SB    25 // orange 
#define PWMCH_L_MOTOR      0 // PWM channel

#define PIN_R_MOTOR_SPEED  2 // blue
#define PIN_R_MOTOR_DIR   15 // white
#define PIN_R_MOTOR_SB    27 // grey
#define PIN_R_MOTOR_SA    26 // purple 
#define PWMCH_R_MOTOR      1 // PWM channel

//* Sensors definitions ***********************************
#define PIN_SENSORL 19
#define PIN_SENSORF 18
#define PIN_SENSORR 5

#define PI 3.1415926535897932384626433832795


#endif