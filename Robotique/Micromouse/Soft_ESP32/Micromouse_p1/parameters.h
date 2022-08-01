

#ifndef PARAMETERS_H
#define PARAMETERS_H

// Robot mechanics
#define WHEEL_DISTANCE 100.
#define WHEEL_CIRCUMFERENCE 35.

//* Motors and encoders definitions ***********************
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
#define PIN_SENSORL 36
#define PIN_SENSORLSF 39
#define PIN_SENSORF 34
#define PIN_SENSORSF 35
#define PIN_SENSORR 32


// Wheel encoders
#define TICKS_PER_TURN 350
#define MM_PER_TICK (WHEEL_CIRCUMFERENCE/TICKS_PER_TURN)


#endif