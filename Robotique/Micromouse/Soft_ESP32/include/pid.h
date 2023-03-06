//*****************************************************************************
//*****************************************************************************
//
// PID - header
//
//*****************************************************************************
//*****************************************************************************

#ifndef _PID_H
#define _PID_H

extern double pidSensors_kp;
extern double pidSensors_ki;
extern double pidSensors_kd;

extern double old_errorSensors;

extern double pidMotors_kp;
extern double pidMotors_ki;
extern double pidMotors_kd;

extern double old_errorMotors;

void initPIDSensors();
double PIDSensors(double error);

void initPIDMotors();
double PIDMotors(double error);

#endif
