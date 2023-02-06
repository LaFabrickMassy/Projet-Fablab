//*****************************************************************************
//*****************************************************************************
//
// PID
// from https://freddy.mudry.org/public/NotesApplications/NAPidAj_06.pdf
//
//*****************************************************************************
//*****************************************************************************

#include "pid.h"

double pid_kp;
double pid_ki;
double pid_kd;

double old_error;

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

