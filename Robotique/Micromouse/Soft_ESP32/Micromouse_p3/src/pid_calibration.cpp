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
    initPID();
}

//*****************************************************************************
//
// Start a PID calibration run
//
//*****************************************************************************
void startPIDRun() {
    Serial.println("startPIDRun()");
    current_state = ROBOT_STATE_RUN;
    motorSetSpeed(&motorL, SPEED_BASE);
    motorSetSpeed(&motorR, SPEED_BASE);
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
    if ( (left_wall_distance < WALL_DISTANCE_LMIN) || 
         (right_wall_distance < WALL_DISTANCE_LMIN) || 
         (front_wall_distance < WALL_DISTANCE_FMIN) ) {
        stopPIDRun();
        return;
    }

    // compute error
    error = left_wall_distance - right_wall_distance;

    // PID
    u = 0.1*PID(error);

    if (u> 0) {
        speed_l = SPEED_BASE * (1-u);
        speed_r = SPEED_BASE;
    }
    else {
        speed_l = SPEED_BASE;
        speed_r = SPEED_BASE * (1+u);
    }

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
    Serial.println("stopPIDRun()");
    current_state = ROBOT_STATE_STOP;
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
    jsonString += "\"mode\":\""+String(current_mode)+"\"";
    jsonString += ",";
    jsonString += "\"state\":\""+String(current_state)+"\"";
    jsonString += ",";
    jsonString += "\"kp\":\""+String(pid_kp)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":\""+String(pid_ki)+"\"";
    jsonString += ",";
    jsonString += "\"kd\":\""+String(pid_kd)+"\"";
    jsonString += ",";
    jsonString += "\"sens_L\":\""+String(distanceSensor1())+"\"";
    #if NB_OF_SENSORS >= 2
    jsonString += ",";
    jsonString += "\"sens_F\":\""+String(distanceSensor2())+"\"";
    #endif
    #if NB_OF_SENSORS >= 3
    jsonString += ",";
    jsonString += "\"sens_R\":\""+String(distanceSensor3())+"\"";
    #endif

    jsonString += "}";

    return jsonString;
}