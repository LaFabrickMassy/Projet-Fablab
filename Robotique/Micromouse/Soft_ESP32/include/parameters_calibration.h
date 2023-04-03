#ifndef _PID_CALIBRATION_H_
#define _PID_CALIBRATION_H_

#define PIDSENSORS_MAX_ERROR 50

extern int stabilised;
extern int pcFlagAuto;

extern int sensorsDataIdx;

void ParamCalInit();
void ParamSensorsStatsInit();
void ParamCalStop();
void ParamCalRunInit();
void ParamCalRunStep();
void ParamCalRotateInit();
void ParamCalRotateStep();
void sensorsStatsUpdate();
String getPIDStatus();
String getTicksStatus();
String getParamCalEncoderResolution();
String getDispErrorsTab();
void logSensorsStats();
void logSensorErrorsTab();

#endif
