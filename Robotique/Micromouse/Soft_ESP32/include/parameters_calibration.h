#ifndef _PID_CALIBRATION_H_
#define _PID_CALIBRATION_H_

#define PIDSENSORS_MAX_ERROR 10

#define PC_STATE_STOP 0
#define PC_STATE_RUN 1
#define PC_STATE_RUN_END 2
#define PC_STATE_ROTATE 3
#define PC_STATE_ROTATE_END 4
#define PC_STATE_CRASH 99

// HMI toggles
#define PC_SELPARAMMODE_KP 0
#define PC_SELPARAMMODE_KI 1
#define PC_SELPARAMMODE_KD 2


//extern int stabilised;
// Flag : automatic return after end of run
extern int FlagAuto;

extern int sensorsDataIdx;

void paramCalInit();
void paramCalStop();
void paramCalStep();
void paramCalRunInit();
void paramCalRunStep();
void paramCalRotateInit();
void paramCalRotateStep();
void paramCalHandleMessage(String message);

void paramSensorsStatsInit();
void sensorsStatsUpdate();
String getPIDStatus();
String getTicksStatus();
String getParamCalEncoderResolution();
String getDispErrorsTab();
void logSensorsStats();
void logSensorErrorsTab();

#endif
