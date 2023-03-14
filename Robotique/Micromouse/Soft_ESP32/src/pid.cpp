//*****************************************************************************
//*****************************************************************************
//
// PID
// from https://freddy.mudry.org/public/NotesApplications/NAPidAj_06.pdf
//
//*****************************************************************************
//*****************************************************************************

#include "parameters.h"
#include "pid.h"

double pidSensors_kp;
double pidSensors_ki;
double pidSensors_kd;

double old_errorSensors;

double pidMotors_kp;
double pidMotors_ki;
double pidMotors_kd;

double old_errorMotors;

//*****************************************************************************
//
//*****************************************************************************
void initPIDSensors() {
    //pidSensors_kp = (double)PIDSENSORS_KP;
    //pidSensors_ki = (double)PIDSENSORS_KI;
    //pidSensors_kd = (double)PIDSENSORS_KD;

    old_errorSensors = 0.;
}

//*****************************************************************************
//
// PID
// parameters:
// y: ouput
// w: setpoint 
//*****************************************************************************
double PIDSensors(double error) {
    double up;
    double ui;
    double ud;

    // Correction factors
    up = pidSensors_kp * error;
    ui = pidSensors_ki * (error + old_errorSensors);
    ud = pidSensors_kd * (error - old_errorSensors);

    // command
    return up + ui + ud;
}

//*****************************************************************************
//
//*****************************************************************************
void initPIDMotors() {
    pidMotors_kp = (double)PIDMOTORS_KP;
    pidMotors_ki = (double)PIDMOTORS_KI;
    pidMotors_kd = (double)PIDMOTORS_KD;

    old_errorMotors = 0.;
}

//*****************************************************************************
//
// PID
// parameters:
// y: ouput
// w: setpoint 
//*****************************************************************************
double PIDMotors(double error) {
    double up;
    double ui;
    double ud;

    // Correction factors
    up = pidMotors_kp * error;
    ui = pidMotors_ki * (error + old_errorMotors);
    ud = pidMotors_kd * (error - old_errorMotors);

    // command
    return up + ui + ud;
}

