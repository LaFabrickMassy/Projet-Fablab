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
#include "wheel_encoder.h"
#include "pid.h"
#include "pid_calibration.h"
#include "micromouse.h"

#define SPEED_BASE 0.7

#define NB_ERRORS 10

double errors[NB_ERRORS];
double total_error;
int nb_steps;
int stabilised;
int fwall_found;
double fwall_refdist; 

long ticks_Lwheel;
long ticks_Rwheel;
long refticks_Lwheel;
long refticks_Rwheel;
long fwall_refticks_Lwheel;
long fwall_refticks_Rwheel;
double ticks_per_mm;

//*****************************************************************************
//
// Initialisation of PID calibration :
// set initial values of kp, ki, kd from defined constants
//
//*****************************************************************************
void PIDRunInit() {
    initPID();
}

//*****************************************************************************
//
// Start a PID calibration run
//
//*****************************************************************************
void PIDRunStart() {
    int i;

    Serial.println("startPIDRun()");
    current_state = ROBOT_STATE_PID_CAL_RUN;
    
    // Initialise error array to 0
    for (i=0;i<NB_ERRORS;i++)
        errors[i] = 0.0;
    nb_steps = 0;
    stabilised = 0;
    total_error = 0.;
    ticks_Lwheel = 0.;
    ticks_Rwheel = 0.;
    fwall_found = 0;

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
void PIDRunStep() {

    double left_wall_distance;
    double right_wall_distance;
    double front_wall_distance;
    double error;
    double u;
    double speed_l, speed_r;
    int i;

    nb_steps ++;

    // get wall distance
    left_wall_distance  = distanceSensor1();
    front_wall_distance = distanceSensor2();
    right_wall_distance = distanceSensor3();

    // Checl wall collision
    if ( (left_wall_distance < WALL_DISTANCE_LMIN) || 
         (right_wall_distance < WALL_DISTANCE_LMIN) || 
         (front_wall_distance < WALL_DISTANCE_FMIN) ) {
        PIDRunStop();
        return;
    }

    // compute error
    error = left_wall_distance - right_wall_distance;

    // update error array and compute total error
    total_error -= errors[0];
    for (i=0;i<NB_ERRORS-1;i++)
        errors[i] = errors[i+1];
    errors[NB_ERRORS-1] = abs(error);
    total_error += abs(error);

    // check if now stabilised
    if ((nb_steps > NB_ERRORS) && (total_error < NB_ERRORS*PID_MAX_ERROR) && (stabilised == 0)) {
        stabilised = 1;
        total_error = 0;
        refticks_Lwheel = -encoderL.count;
        refticks_Rwheel = encoderR.count;
    }
    if (stabilised) {
        if ((fwall_found == 0) && (front_wall_distance < 1000)) {
            fwall_found = 1;
            fwall_refdist = front_wall_distance;
            fwall_refticks_Lwheel = -encoderL.count;
            fwall_refticks_Rwheel = -encoderL.count;
        }
        ticks_Lwheel = -encoderL.count - refticks_Lwheel;
        ticks_Rwheel = encoderR.count - refticks_Rwheel;
        ticks_per_mm = 100*(fwall_refdist-front_wall_distance) * 2/(-encoderL.count - fwall_refticks_Lwheel + encoderR.count - fwall_refticks_Rwheel);
    }
    

    // PID
    u = 0.01*PID(error);

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
void PIDRunStop() {

    Serial.println("stopPIDRun()");
    current_state = ROBOT_STATE_STOP;
    motorSetSpeed(&motorL, 0);
    motorSetSpeed(&motorR, 0);
    
}

//*****************************************************************************
//
// Go gently to the wall and half turn :
//
//*****************************************************************************
void PIDRunGoToWall() {
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
    jsonString += ",";
    jsonString += "\"ticks_L\":\""+String(ticks_Lwheel)+"\"";
    jsonString += ",";
    jsonString += "\"ticks_R\":\""+String(ticks_Rwheel)+"\"";
    jsonString += ",";
    jsonString += "\"err\":\""+String(total_error)+"\"";
    jsonString += ",";
    jsonString += "\"enc_L\":\""+String(encoderL.count)+"\"";
    jsonString += ",";
    jsonString += "\"enc_R\":\""+String(encoderR.count)+"\"";
    jsonString += ",";
    jsonString += "\"tick_pmm\":\""+String(ticks_per_mm)+"\"";
    
    jsonString += "}";

    return jsonString;
}

//********************************************************************
// 
//********************************************************************
String getPIDCalParameters() {
    String jsonString;

    jsonString =  "{";
    jsonString += "\"kp\":"+String(pid_kp)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":"+String(pid_ki)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":"+String(pid_ki)+"\"";

    jsonString += "}";
    return jsonString;
}

