//*****************************************************************************
//*****************************************************************************
//
// PID - header
//
//*****************************************************************************
//*****************************************************************************

#ifndef _PID_H
#define PID_H

extern double pid_kp;
extern double pid_ki;
extern double pid_kd;
extern double old_error;

void initPID();
double PID(double error);

#endif
