

#ifndef _DISTANCE_SENSOR_
#define _DISTANCE_SENSOR_

#include "parameters.h"

#define SENSOR_MAX 1000

//********************************************************************
// typedef for sensor
typedef struct {
	int pin;
	unsigned long rising_time;
	unsigned long duration;
    unsigned int count;
    unsigned int newvalue;
}
sensor_t;

extern sensor_t sensor1;
void setupSensor1(int pin);
long distanceSensor1();
void resetSensor1();

#if NB_OF_SENSORS >= 2
extern sensor_t sensor2;
void setupSensor2(int pin);
long distanceSensor2();
void resetSensor2();
#endif

#if NB_OF_SENSORS >= 3
extern sensor_t sensor3;
void setupSensor3(int pin);
long distanceSensor3();
void resetSensor3();
#endif

#if NB_OF_SENSORS >= 4
extern sensor_t sensor4;
void setupSensor4(int pin);
long distanceSensor4();
void resetSensor4();
#endif

#if NB_OF_SENSORS >= 5
extern sensor_t sensor5;
void setupSensor5(int pin);
long distanceSensor5();
void resetSensor5();
#endif




#endif