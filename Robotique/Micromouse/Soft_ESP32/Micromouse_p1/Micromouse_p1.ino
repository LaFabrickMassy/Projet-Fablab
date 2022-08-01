#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <SPIFFS.h>
#include "motor.h"
#include "parameters.h"
#include "wheel_encoder.h"


#define DEBUG


motor_t motorL; // left motor
motor_t motorR; // right motor

//* Web server definitions ********************************
const char* ssid = "ESP32";
const char* password = NULL;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");



// robot status
double speed;
double turn;
double pos_x, pos_y, heading;
double mov_x, mov_y, mov_h; // x, y and heading of movement

long millis_count;
long millis_current;

// Initialize SPIFFS
void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
   Serial.println("SPIFFS mounted successfully");
  }
}

/**********************************************************
* setup
**********************************************************/
void setup()
{
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
    setupEncoderR(PIN_L_MOTOR_SA, PIN_L_MOTOR_SB);
    setupEncoderL(PIN_R_MOTOR_SA, PIN_R_MOTOR_SB);

    Serial.begin(115200);

    initFS();

	// Setup motors *******************
	Serial.println("Setup Motors ******************");
	err = motorSetup(&motorL, PIN_L_MOTOR_SPEED, PIN_L_MOTOR_DIR, 0, PWMCH_L_MOTOR);
	Serial.print(err);
	if (!err)
	{
		Serial.println("Left motor initialised OK");
	}
	else
		Serial.println("Left motor not initialised");

	err = motorSetup(&motorR, PIN_R_MOTOR_SPEED, PIN_R_MOTOR_DIR, 1, PWMCH_R_MOTOR);
	Serial.print(err);
	if (!err)
	{
		Serial.println("right motor initialised OK");
	}
	else
		Serial.println("right motor not initialised");
	
    initWiFi();
    delay(500);
    initWebServer();
    
	// Setup sensors ******************
	setup_sensor1(PIN_SENSORL); // Left sensor
	setup_sensor2(PIN_SENSORSF); // Left slanted sensor
	setup_sensor3(PIN_SENSORF); // front sensor
	setup_sensor4(PIN_SENSORSF); // right slanted sensor
	setup_sensor5(PIN_SENSORR); // right sensor
    
    millis_count=millis();
}

/**********************************************************
* loop
**********************************************************/
void loop()
{
    delay(100);
    cleanupClients();
    
    motorControl();
    //computeMove();
    
        millis_current = millis();
    if (millis_current > (millis_count + 1000))
    {
        millis_count = millis_current;
        notifyClients(getRobotStatus());
    }
}