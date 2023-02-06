

#ifndef _DISTANCE_SENSOR
#define _DISTANCE_SENSOR

//********************************************************************
// typedef for sensor
typedef struct {
	int pin;
	unsigned long rising_time;
	unsigned long duration;
    unsigned int count;
}
sensor_t;

extern sensor_t sensor1;
void setupSensor1(int pin);
long distanceSensor1();

#if NB_OF_SENSORS >= 2
extern sensor_t sensor2;
void setupSensor2(int pin);
long distanceSensor2();
#endif

#if NB_OF_SENSORS >= 3
extern sensor_t sensor3;
void setupSensor3(int pin);
long distanceSensor3();
#endif

#if NB_OF_SENSORS >= 4
extern sensor_t sensor4;
void setupSensor4(int pin);
long distanceSensor4();
#endif

#if NB_OF_SENSORS >= 5
extern sensor_t sensor5;
void setupSensor5(int pin);
long distanceSensor5();
#endif




#endif