

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

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
#define NB_OF_SENSORS 3
#define PIN_SENSORL 19
#define PIN_SENSORF 18
#define PIN_SENSORR 5

// Wall minimal distance for crash detection
#define WALL_DISTANCE_FMIN 50 // front distance
#define WALL_DISTANCE_LMIN 30 // border distance
#define WALL_DISTANCE_MAX 75. // max wall distance to detect holes

// Robot mechanics
// left and right sensors angle in degrees
#define SIDE_SENSORS_ANGLE 75 
#define ROBOT_WHEEL_DISTANCE 93.
#define ENCL_RESOL 0.250 //0.225
#define ENCR_RESOL 0.225
#define SPEED_MAX 10 // maximal speed, in mm/s
#define TACHOMETER_PERIOD_US 50000 // 20 ms
#define TACHOMETER_PERIOD_SEC ((double)TACHOMETER_PERIOD_US/1000000.)

// PID Controllers parameters
#define SPEED_PID_KP 0.
#define SPEED_PID_KI 0.
#define SPEED_PID_KD 0.
#define HEAD_PID_KP 0.0022
#define HEAD_PID_KI 0.0005
#define HEAD_PID_KD 0.

// Trace level:
// 1: IHM, state change
// 2: robot control init and stop
#define TRACE_LEVEL 2
#define LOG_SENSORPID_ERRORS 1

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
#define PIDSENSORS_KI 0.002
#define PIDSENSORS_KD 0
#define PIDMOTORS_KP 0
#define PIDMOTORS_KI 0
#define PIDMOTORS_KD 0

// Sensors stats
#define SENSORS_STDDEV 2.35 // Standard deviation of e=dL-dR

#define PI 3.1415926535897932384626433832795
#define PIs2 (PI/2.)
#define PIs4 (PI/4.)


#endif