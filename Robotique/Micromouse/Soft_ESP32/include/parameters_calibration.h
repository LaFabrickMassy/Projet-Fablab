#ifndef _PID_CALIBRATION_H_
#define _PID_CALIBRATION_H_

#define PIDSENSORS_MAX_ERROR 50

extern int stabilised;

void ParamCalInit();
void ParamCalStop();
void ParamCalRunInit();
void ParamCalRunStep();
void ParamCalRotateInit();
void ParamCalRotateStep();
String getPIDStatus();
String getTicksStatus();
String getParamCalEncoderResolution();
void logSensorErrorsTab();
#endif
