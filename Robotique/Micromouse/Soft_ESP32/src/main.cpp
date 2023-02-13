#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <SPIFFS.h>
#include "parameters.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "distance_sensor.h"
#include "webserver.h"
#include "simple_run.h"
#include "pid_calibration.h"
#include "micromouse.h"


#define DEBUG


motor_t motorL; // left motor
motor_t motorR; // right motor

int current_mode = ROBOT_MODE_STOP;
int current_state = ROBOT_STATE_STOP;

#ifdef WIFI_MODE_AP
//* Web server definitions ********************************
const char* ssid = "ESP32";
const char* password = NULL;
#else
const char* ssid = "";
const char* password = "";
#endif




// robot status
double speed;
double turn;
double pos_x, pos_y;
double heading;
// x, y and heading of movement
double mov_x, mov_y;
double mov_h; 

long millis_count_status;
long millis_count_mode;
long millis_current;

// Initialize SPIFFS
void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else {
   Serial.println("SPIFFS mounted successfully");
  }
}

/**********************************************************
* setup
**********************************************************/
void setup() {
    int err;
  
    speed = 0.;
    turn = 0.;
    pos_x = 0.;
    pos_y = 0.;
    heading = 0.;
    
    // set motors to 0
    pinMode(PIN_L_MOTOR_SPEED, OUTPUT);
    digitalWrite(PIN_L_MOTOR_SPEED, LOW);
    pinMode(PIN_R_MOTOR_SPEED, OUTPUT);
    digitalWrite(PIN_R_MOTOR_SPEED, LOW);
    
    // set encoders
    /* DEBUG
    setupEncoderL(PIN_L_MOTOR_SA, PIN_L_MOTOR_SB);
    setupEncoderR(PIN_R_MOTOR_SA, PIN_R_MOTOR_SB);
    */
    setupEncoderR(PIN_L_MOTOR_SA, PIN_L_MOTOR_SB, 1);
    setupEncoderL(PIN_R_MOTOR_SA, PIN_R_MOTOR_SB, 0);

    Serial.begin(115200);

    initFS();

	  // Setup motors *******************
	Serial.println("Setup Motors ******************");
  	err = motorSetup(&motorL, PIN_L_MOTOR_SPEED, PIN_L_MOTOR_DIR, 1, PWMCH_L_MOTOR);
	Serial.print(err);
	if (!err) {
        Serial.println("Left motor initialised OK");
    }
    else {
		Serial.println("Left motor not initialised");
    }

	err = motorSetup(&motorR, PIN_R_MOTOR_SPEED, PIN_R_MOTOR_DIR, 1, PWMCH_R_MOTOR);
	Serial.print(err);
	if (!err) {
		Serial.println("right motor initialised OK");
	}
	else
		Serial.println("right motor not initialised");
	
    initWiFi();
    delay(500);
    initWebServer();
    
	// Setup sensors ******************
	setupSensor1(PIN_SENSORL); // Left sensor
	setupSensor2(PIN_SENSORF); // Left slanted sensor
	setupSensor3(PIN_SENSORR); // front sensor
	//SetupSensor4(PIN_SENSORRS); // right slanted sensor
	//SetupSensor5(PIN_SENSORR); // right sensor
    
    // Initialise position and movements
    current_mode == ROBOT_MODE_STOP;
    resetEncoderL();
    resetEncoderR();
    speed = 0;
    turn = 0;
    pos_x = 0;
    pos_y = 0;
    heading = 0;



    millis_current=millis();
    millis_count_mode = millis_current;
    millis_count_status = millis_current;
}

/**********************************************************
* loop
**********************************************************/
void loop()
{
    millis_current = millis();

    
    // print mode on console
    if (millis_current - millis_count_mode > 2000) {
        millis_count_mode = millis_current;
        if (current_mode == ROBOT_MODE_STOP) {
            Serial.println("Mode: STOP");     
            //Serial.println(sensor1.count);
            //Serial.println(sensor2.count);
            //Serial.println(sensor3.count);
        }       
        else if (current_mode == ROBOT_MODE_SIMPLE_RUN)
            Serial.println("Mode: Simple run");
        else if (current_mode == ROBOT_MODE_PID_CAL)
            Serial.println("Mode: PID Calibration");
        else
            Serial.println("Mode: UNKNOWN");
    }
    if (millis_current - millis_count_status > 1000) {
        millis_count_status = millis_current;        
        //motorControl();
        if (current_mode == ROBOT_MODE_SIMPLE_RUN)
            notifyClients(getRobotStatus());
        if (current_mode == ROBOT_MODE_PID_CAL)
            notifyClients(getPIDStatus());

        #ifdef DEBUG_SENSORS
        // print measured distance for sensor 1
        Serial.print(distanceSensor1());
        Serial.print(" - ");
        Serial.print(sensor1.duration);
        Serial.print(" | ");
        #if NB_OF_SENSORS >= 2
        // print measured distance for sensor 2
        Serial.print(distanceSensor2());
        Serial.print(" - ");
        Serial.print(sensor2.Duration);
        Serial.print(" | ");
        #endif
        #if NB_OF_SENSORS >= 3
        // print measured distance for sensor 3
        Serial.print(distanceSensor3());
        Serial.print(" - ");
        Serial.print(sensor3.duration);
        Serial.print(" | ");
        #endif
        #if NB_OF_SENSORS >= 4
        // print measured distance for sensor 4
        Serial.print(distanceSensor4());
        Serial.print(" - ");
        Serial.print(sensor4.duration);
        Serial.print(" | ");
        #endif
        #if NB_OF_SENSORS >= 5
        // print measured distance for sensor 5
        Serial.print(distanceSensor5());
        Serial.print(" - ");
        Serial.print(sensor5.duration);
        #endif
        Serial.println("");

        #endif // DEBUG_SENSORS
    }

    if (current_mode == ROBOT_MODE_SIMPLE_RUN) {
        if (current_state == ROBOT_STATE_SR_RUN) {
            simpleRunStep();
        }
    }
    if (current_mode == ROBOT_MODE_PID_CAL) {
        if (current_state == ROBOT_STATE_PID_CAL_RUN) {
            PIDRunStep();
        }
        if (current_state == ROBOT_STATE_PID_CAL_END) {
            PIDRunGoToWall();
        }
    }
}