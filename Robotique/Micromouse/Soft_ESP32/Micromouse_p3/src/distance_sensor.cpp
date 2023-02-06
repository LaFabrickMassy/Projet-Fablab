//********************************************************************
// Pololu distance sensor 50cm, product item #4064
// https://www.pololu.com/product/4064
// Circuit based on ST VL53L0X ToF ranging sensor
//
// Note: it would be more elegant with a class, but ISR must be 
// a static function, and object methods are not static. So we need
// do define static functions.
//
// To add more sensors, just copy/paste and replace sensor1 with sensorX.
//
// Usage:
//   Setup the sensor read functions with setup_sensorX(pin) 
//   read the measured distance with distance_sensorX()
//
//********************************************************************

#include <Arduino.h>
#include "parameters.h"
#include "distance_sensor.h"

//********************************************************************
// Sensor 1
//********************************************************************
sensor_t sensor1;

//********************************************************************
// ISR for sensor 1 : read pulse duration
void IRAM_ATTR isr_sensor1() 
{
    sensor1.count++;
	if (digitalRead(sensor1.pin))
		sensor1.rising_time = micros();
	else
	{
		sensor1.duration = micros() - sensor1.rising_time;
	}
}

//********************************************************************
// Setup sensor 1 : 
// Set pin of sensor and attach ISR
//
// @param pin
void setupSensor1(int pin)
{
	sensor1.pin = pin;
	sensor1.rising_time = 0;
	sensor1.duration = 0;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor1, CHANGE);
}

//********************************************************************
// Read measured distance of sensor 1
long distanceSensor1()
{
	if(sensor1.duration > 1850)
		return -1;
	else if(sensor1.duration < 1000)
		return 0;
	else
		return (sensor1.duration-1000)*3/4;
}

#if NB_OF_SENSORS >= 2
//********************************************************************
// Sensor 2
//********************************************************************
sensor_t sensor2;

//********************************************************************
void IRAM_ATTR isr_sensor2() 
{
    sensor2.count++;
    if (digitalRead(sensor2.pin))
		sensor2.rising_time = micros();
	else
	{
		sensor2.duration = micros() - sensor2.rising_time;
	}
}

//********************************************************************
void setupSensor2(int pin)
{
	sensor2.pin = pin;
	sensor2.rising_time = 0;
	sensor2.duration = 0;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor2, CHANGE);
}

//********************************************************************
long distanceSensor2()
{
	if(sensor2.duration > 1850)
		return -1;
	else if(sensor2.duration < 1000)
		return 0;
	else
		return (sensor2.duration-1000)*3/4;
}
#endif

#if NB_OF_SENSORS >= 3
//********************************************************************
// Sensor 3
//********************************************************************
sensor_t sensor3;

//********************************************************************
void IRAM_ATTR isr_sensor3() 
{
    sensor3.count++;    
	if (digitalRead(sensor3.pin))
		sensor3.rising_time = micros();
	else
	{
		sensor3.duration = micros() - sensor3.rising_time;
	}
}

//********************************************************************
void setupSensor3(int pin)
{
	sensor3.pin = pin;
	sensor3.rising_time = 0;
	sensor3.duration = 0;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor3, CHANGE);
}

//********************************************************************
long distanceSensor3()
{
	if(sensor3.duration > 1850)
		return -1;
	else if(sensor3.duration < 1000)
		return 0;
	else
		return (sensor3.duration-1000)*3/4;
}
#endif

#if NB_OF_SENSORS >= 4
//********************************************************************
// Sensor 4
//********************************************************************
sensor_t sensor4;

//********************************************************************
void IRAM_ATTR isr_sensor4() 
{
	if (digitalRead(sensor4.pin))
		sensor4.rising_time = micros();
	else
	{
		sensor4.duration = micros() - sensor4.rising_time;
	}
}

//********************************************************************
void setupSensor4(int pin)
{
	sensor4.pin = pin;
	sensor4.rising_time = 0;
	sensor4.duration = 0;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor4, CHANGE);
}

//********************************************************************
long distanceSensor4()
{
	if(sensor4.duration > 1850)
		return -1;
	else if(sensor4.duration < 1000)
		return 0;
	else
		return (sensor4.duration-1000)*3/4;
}
#endif

#if NB_OF_SENSORS >= 5
//********************************************************************
// Sensor 5
//********************************************************************
sensor_t sensor5;

//********************************************************************
void IRAM_ATTR isr_sensor5() 
{
	if (digitalRead(sensor5.pin))
		sensor5.rising_time = micros();
	else
	{
		sensor5.duration = micros() - sensor5.rising_time;
	}
}

//********************************************************************
void setupSensor5(int pin)
{
	sensor5.pin = pin;
	sensor5.rising_time = 0;
	sensor5.duration = 0;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor5, CHANGE);
}

//********************************************************************
long distanceSensor5()
{
	if(sensor5.duration > 1850)
		return -1;
	else if(sensor5.duration < 1000)
		return 0;
	else
		return (sensor5.duration-1000)*3/4;
}
#endif
