#include <Arduino.h>
#include <SPIFFS.h>
#include "parameters.h"
#include "maze.h"
#include "robot_hw.h"
#include "robot_speed_controller.h"
#include "robot_controller.h"
#include "robot.h"
#include "webserver.h"
#include "test_drive.h"
#include "parameters_calibration.h"
#include "explore.h"
#include "hmi.h"
#include "logutils.h"
#include "micromouse.h"


String debug_message;

#ifdef WIFI_MODE_AP
//* Web server definitions ********************************
const char* ssid = "ESP32";
const char* password = NULL;
#else
const char* ssid = "";
const char* password = "";
#endif

Maze maze;
Robot robot;



long millis_count_status;
long millis_count_mode;
long millis_current;
long millis_count_0100; // counter 100 ms
long millis_count_0500; // counter 500 ms
long millis_count_1000; // counter 1s
long millis_count_2000; // counter 2s 

long loops_count;

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
  
    Serial.begin(115200);
    Serial.println("Started");

    robot.Init();

    Serial.println("Started 2");

    initFS();

    Serial.println("Started 3");

    initWiFi();
    delay(500);

    Serial.println("Started 4");

    initWebServer();
    
    Serial.println("Started 5");

    millis_current = millis();
    millis_count_mode = millis_current;
    millis_count_status = millis_current;

    //dumpToSerial();
    Serial.println("End of setup");

    hmiInit();

    Serial.println("Started 6");

}

/**********************************************************
* loop
**********************************************************/
void loop()
{
    millis_current = millis();

    // 100ms operations
    if (millis_current - millis_count_0100 > 100) {
        millis_count_0100 = millis_current;

        // operations
    }

    // 500ms operations
    if (millis_current - millis_count_0500 > 500) {
        millis_count_0500 = millis_current;

        // operations
    }

    // 1s operations
    if (millis_current - millis_count_1000 > 1000) {
        millis_count_1000 = millis_current;

        // operations
    }

    // 2s operations
    if (millis_current - millis_count_2000 > 2000) {
        millis_count_2000 = millis_current;

        // operations
        //logWrite(robot.String_State());
        notifyClients(robot.controller.JSON_ControllerStatus());
        logWrite(robot.controller.String_ControllerStatus());
        logWrite("SpeedCtrl L:"+robot.controller.speed_controllerL.String_SpeedPID_Params());
        logWrite("SpeedCtrl R:"+robot.controller.speed_controllerL.String_SpeedPID_Params());

    }

    hmiStep();
}