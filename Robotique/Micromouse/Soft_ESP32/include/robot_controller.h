#ifndef _ROBOT_CONTROLLER_H
#define _ROBOT_CONTROLLER_H

#ifdef DEBUG
extern double debug_nbticksL;
extern double debug_nbticksR;
#endif

extern double speed_targetL;
extern double speed_targetR;

void rotationInit(double degrees, double speed);
int rotationStep();

#endif