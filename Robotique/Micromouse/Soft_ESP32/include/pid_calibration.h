#ifndef _PID_CALIBRATION_H_
#define _PID_CALIBRATION_H_

extern int stabilised;

void PIDRunInit();
void PIDRunStart();
void PIDRunStep();
void PIDRunStop();
String getPIDStatus();
void PIDRunGoToWall();
#endif
