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

double pid_kp;
double pid_ki;
double pid_kd;

double old_error;

//*****************************************************************************
//
//*****************************************************************************
void initPID() {
    pid_kp = (double)PID_INITIAL_KP;
    pid_ki = (double)PID_INITIAL_KI;
    pid_kd = (double)PID_INITIAL_KD;

    old_error = 0.;
}

//*****************************************************************************
//
// PID
// parameters:
// y: ouput
// w: setpoint 
//*****************************************************************************
double PID(double error) {
    double up;
    double ui;
    double ud;

    // Correction factors
    up = pid_kp * error;
    ui = pid_ki * (error + old_error);
    ud = pid_kd * (error - old_error);

    // command
    return up + ui + ud;
}

