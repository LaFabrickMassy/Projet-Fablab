//*****************************************************************************
//*****************************************************************************
//
// PID calibration
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include "parameters.h"
#include "motor.h"
#include "distance_sensor.h"
#include "pid.h"
#include "pid_calibration.h"
#include "micromouse.h"

#define SPEED_BASE 0.5

//*****************************************************************************
//
// Initialisation of PID calibration :
// set initial values of kp, ki, kd from defined constants
//
//*****************************************************************************
void initPIDRun()
{
    pid_kp = (double)PID_INITIAL_KP;
    pid_ki = (double)PID_INITIAL_KI;
    pid_kd = (double)PID_INITIAL_KD;

    old_error = 0.;
}

//*****************************************************************************
//
// Start a PID calibration run
//
//*****************************************************************************
void startPIDRun() {
    motorSetSpeed(&motorL, speed_l);
    motorSetSpeed(&motorR, speed_r);
}

//*****************************************************************************
//
// Continue a PID calibration run :
// Update motor speed from sensors
// Stop if wall distance is too low
//
//*****************************************************************************
void stepPIDRun() {

    double left_wall_distance;
    double right_wall_distance;
    double front_wall_distance;
    double error;
    double u;
    double speed_l, speed_r;

    // get wall distance
    left_wall_distance  = distanceSensor1();
    front_wall_distance = distanceSensor2();
    right_wall_distance = distanceSensor3();

    // Checl wall collision
    if ( (left_wall_distance < WALL_DISTANCE_MIN) || (right_wall_distance < WALL_DISTANCE_MIN) || (front_wall_distance < WALL_DISTANCE_MIN) )
        stopPIDRun();

    // compute error
    error = left_wall_distance - right_wall_distance;

    // PID
    u = PID(error);

    speed_l = SPEED_BASE * (1+u);
    speed_r = SPEED_BASE * (1-u);
    motorSetSpeed(&motorL, speed_l);
    motorSetSpeed(&motorR, speed_r);

    #undef SPEED_BASE
}

//*****************************************************************************
//
// Stop a PID calibration run :
// Stop motors 
//
//*****************************************************************************
void stopPIDRun() {
    motorSetSpeed(&motorL, 0);
    motorSetSpeed(&motorR, 0);
}

//*****************************************************************************
//
// Return PID values in json format
//
//*****************************************************************************
String getPIDStatus() {
    String jsonString;
    
    jsonString = "{";
    jsonString += "\"kp\":\""+String(pid_kp)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":\""+String(pid_ki)+"\"";
    jsonString += ",";
    jsonString += "\"kd\":\""+String(pid_kd)+"\"";

    jsonString += "}";
    return jsonString;

}