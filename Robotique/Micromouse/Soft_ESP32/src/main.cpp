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
#include "test_drive.h"
#include "parameters_calibration.h"
#include "robot_controller.h"
#include "logutils.h"
#include "micromouse.h"


motor_t motorL; // left motor
motor_t motorR; // right motor

int current_mode = ROBOT_MODE_STOP;
int current_state = ROBOT_STATE_STOP;
String debug_message;

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
    Serial.println("Started");

    initFS();

	  // Setup motors *******************
	logWrite("Setup Motors ******************");
  	err = motorSetup(&motorL, PIN_L_MOTOR_SPEED, PIN_L_MOTOR_DIR, 1, PWMCH_L_MOTOR);
	if (!err) {
        logWrite("Left motor initialised OK");
    }
    else {
		logWrite("Left motor not initialised");
    }

	err = motorSetup(&motorR, PIN_R_MOTOR_SPEED, PIN_R_MOTOR_DIR, 1, PWMCH_R_MOTOR);
	if (!err) {
		logWrite("right motor initialised OK");
	}
	else
		logWrite("right motor not initialised");
	
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

    dumpToSerial();
    logWrite("End of setup");
}

/**********************************************************
* loop
**********************************************************/
void loop()
{
    millis_current = millis();

    if (millis_current - millis_count_status > 5000) {
        millis_count_status = millis_current;        

        //logWrite("Loop ----------------");
        logRobotState();
        if (current_mode == ROBOT_MODE_TEST_DRIVE)
            notifyClients(getRobotStatus());
        if (current_mode == ROBOT_MODE_PARAM)
            notifyClients(getPIDStatus());
            //notifyClients("{\"trailtext\":\""+String(current_mode)+"/"+String(current_state)+" "+debug_message+"\"}");
        #ifdef DEBUG_SENSORS
        // print measured distance for sensor 1
        logWrite(distanceSensor1());
        logWrite(" - ");
        logWrite(sensor1.duration);
        logWrite(" | ");
        #if NB_OF_SENSORS >= 2
        // print measured distance for sensor 2
        logWrite(distanceSensor2());
        logWrite(" - ");
        logWrite(sensor2.Duration);
        logWrite(" | ");
        #endif
        #if NB_OF_SENSORS >= 3
        // print measured distance for sensor 3
        logWrite(distanceSensor3());
        logWrite(" - ");
        logWrite(sensor3.duration);
        logWrite(" | ");
        #endif
        #if NB_OF_SENSORS >= 4
        // print measured distance for sensor 4
        logWrite(distanceSensor4());
        logWrite(" - ");
        logWrite(sensor4.duration);
        logWrite(" | ");
        #endif
        #if NB_OF_SENSORS >= 5
        // print measured distance for sensor 5
        logWrite(distanceSensor5());
        logWrite(" - ");
        logWrite(sensor5.duration);
        #endif
        logWrite("");

        #endif // DEBUG_SENSORS
    }

    switch(current_mode) {
        case ROBOT_MODE_STOP:
            break;
        case ROBOT_MODE_TEST_DRIVE:
            switch(current_state) {
                case ROBOT_STATE_STOP:
                    // Stopped, do nothing
                    break;
                case ROBOT_STATE_CRASH:
                case ROBOT_STATE_RUN_END:
                case ROBOT_STATE_ROTATE_END:
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: CRASH/END, testDriveStop() == START");
                    #endif
                    testDriveStop();
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: testDriveStop() == END");
                    #endif
                    break;
                case ROBOT_STATE_RUN:
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: RUN, testDriveRunStep() == START");
                    #endif
                    testDriveRunStep();
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: testDriveRunStep() == END");
                    #endif
                    break;
                case ROBOT_STATE_ROTATE:
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: RUN, testDriveRotateStep() == START");
                    #endif
                    testDriveRotateStep();
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: testDriveRotateStep() == END");
                    #endif
                    break;
                default:
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: Unknown state in mode TEST_DRIVE: "+String(current_state));
                    #endif
                    break;
            }
            break;
        case ROBOT_MODE_PARAM:
            switch(current_state) {
                case ROBOT_STATE_STOP:
                    break;
                case ROBOT_STATE_CRASH:
                    ParamCalStop();
                    current_state = ROBOT_MODE_STOP;
                    break;
                case ROBOT_STATE_RUN:
                    ParamCalRunStep();
                    break;
                case ROBOT_STATE_RUN_END:
                    #if LOG_SENSORPID_ERRORS > 0
                    //logSensorErrorsTab();
                    #endif
                    notifyClients(getDispErrorsTab());
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: RunEnd, ParamCalRotateInit() == START");
                    #endif
                    ParamCalRotateInit();
                    current_state = ROBOT_STATE_ROTATE;
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: ParamCalRotateInit() == END");
                    #endif
                    break;
                case ROBOT_STATE_ROTATE:
                    ParamCalRotateStep();
                    break;
                case ROBOT_STATE_ROTATE_END:
                    if (pcFlagAuto) {
                        #if TRACE_LEVEL >= 2
                        logWrite("loop: RotateEnd, ParamCalRunInit() == START");
                        logRobotState();
                        #endif
                        ParamCalRunInit();
                        #if TRACE_LEVEL >= 2
                        logWrite("loop: ParamCalRunInit() == END");
                        #endif
                    }
                    else {
                        #if TRACE_LEVEL >= 2
                        logWrite("loop: RotateEnd, ParamCalRunInit() == START");
                        logRobotState();
                        #endif
                        current_state = ROBOT_STATE_STOP;
                        #if TRACE_LEVEL >= 2
                        logWrite("loop: ParamCalRunInit() == END");
                        #endif
                    }
                    break;
                default:
                    #if TRACE_LEVEL >= 2
                    logWrite("loop: Unknown state in mode PARAM: "+String(current_state));
                    #endif
                    break;
            }
            break;
        default:
            #if TRACE_LEVEL >= 2
            logWrite("loop: Unknown mode: "+String(current_mode));
            #endif
            break;
    }
    #ifdef TO_REMOVE
    if (current_mode == ROBOT_MODE_TEST_DRIVE) {
        if (current_state == ROBOT_STATE_RUN) {
            testDriveRunStep();
        }
        else if (current_state == ROBOT_STATE_RUN_END) {
            #if TRACE_LEVEL >= 2
            logWrite("loop: RunEnd, testDriveStop() == START");
            #endif
            testDriveStop();
            #if TRACE_LEVEL >= 2
            logWrite("loop: testDriveStop() == END");
            #endif
        }
        else if (current_state == ROBOT_STATE_ROTATE) {
            testDriveRotateStep();
        }
        else if (current_state == ROBOT_STATE_ROTATE_END) {
            #if TRACE_LEVEL >= 2
            logWrite("loop: RotateEnd, testDriveStop() == START");
            #endif
            testDriveStop();
            #if TRACE_LEVEL >= 2
            logWrite("loop: testDriveStop() == END");
            #endif
        }
    }
    if (current_mode == ROBOT_MODE_PARAM) {
        if (current_state == ROBOT_STATE_RUN) {
            // Run step
            // RunStep set state to RUN_END when wall 
            ParamCalRunStep();
        }
        else if (current_state == ROBOT_STATE_RUN_END) {
            // end of run, start rotate
            // RotateInit set state to ROTATION
            #if LOG_SENSORPID_ERRORS > 0
            //logSensorErrorsTab();
            #endif
            #if TRACE_LEVEL >= 2
            logWrite("loop: RunEnd, ParamCalRotateInit() == START");
            #endif
            ParamCalRotateInit();
            current_state = ROBOT_STATE_STOP;
            #if TRACE_LEVEL >= 2
            logWrite("loop: ParamCalRotateInit() == END");
            #endif

        }
        else if (current_state == ROBOT_STATE_ROTATE) {
            #ifdef DEBUG
            debug_nbticksL += (abs(speed_targetL)*0.0033);
            debug_nbticksR += (abs(speed_targetR)*0.0033);
            #endif
            // Rotation step
            // RotationStep set state to ROTATION_END when target angle reached
            ParamCalRotateStep();
        }
        else if (current_state == ROBOT_STATE_ROTATE_END) {
            // end of rotation, start run
            // RunInit set state to RUN
            #if TRACE_LEVEL >= 2
            logWrite("loop: RotateEnd, ParamCalRunInit() == START");
            logRobotState();
            #endif
            ParamCalRunInit();
            #if TRACE_LEVEL >= 2
            logWrite("loop: ParamCalRunInit() == END");
            #endif
        }
        else if (current_state == ROBOT_STATE_CRASH) {
            // Crash
            #if TRACE_LEVEL >= 2
            logWrite("loop: Crash, ParamCalStop() == START");
            #endif
            ParamCalStop();
            #if TRACE_LEVEL >= 2
            logWrite("loop: ParamCalStop() == END");
            #endif
        }
    }
    #endif
}