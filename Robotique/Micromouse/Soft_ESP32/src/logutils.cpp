#include "Arduino.h"
#include "SPIFFS.h"
#include "motor.h"
#include "webserver.h"
#include "micromouse.h"
#include "logutils.h"

File logfd;
char buffer[40];

//********************************************************************
// 
//********************************************************************
String logTimeStamp() {
    long timestamp;
    int ts_sec;
    int ts_millis;
    String s_sec;
    String s_millis;

    timestamp = millis();
    ts_sec = timestamp / 1000;
    ts_millis = timestamp % 1000;

    s_sec = String(ts_sec);
    s_millis = String(ts_millis);

    while(s_sec.length() < 5) {
        s_sec = "0"+s_sec;
    }
    while(s_millis.length() < 3) {
        s_millis = "0"+s_millis;
    }
    
    return s_sec+"."+s_millis+" ";
}

//********************************************************************
// 
//********************************************************************
void logWrite(String s) {
    notifyClients("{\"log\":\""+logTimeStamp()+s+"\"}\n");
}

//********************************************************************
// 
//********************************************************************
void dumpToSerial() {
    logfd.close();
    logfd = SPIFFS.open(LOGFNAME, FILE_READ);
    if(!logfd) {
        Serial.println("Error while opening logfile");
    }
    Serial.println("Dump of logfile ------------");
    while(logfd.available()) {
        Serial.write(logfd.read());
   }
    Serial.println("Dump of logfile end --------");
    logfd.close();
    logfd = SPIFFS.open(LOGFNAME, FILE_APPEND);
}

//********************************************************************
// 
//********************************************************************
void logRobotState() {

    String s_mode;      
    String s_state;

    if (current_mode == ROBOT_MODE_STOP) 
        s_mode = "STOP";     
    else if (current_mode == ROBOT_MODE_TEST_DRIVE)
        s_mode = "Test Drive";
    else if (current_mode == ROBOT_MODE_PARAM)
        s_mode = "Parameters Calibration";
    else
        s_mode = "UNKNOWN";

    if (current_state == ROBOT_STATE_STOP)
        s_state = "STOP";
    else if (current_state == ROBOT_STATE_RUN)
        s_state = "RUN";
    else if (current_state == ROBOT_STATE_RUN_END)
        s_state = "RUN END";
    else if (current_state == ROBOT_STATE_ROTATE)
        s_state = "ROTATE";
    else if (current_state == ROBOT_STATE_ROTATE_END)
        s_state = "ROTATE END";
    else if (current_state == ROBOT_STATE_TURN)
        s_state = "TURN";
    else if (current_state == ROBOT_STATE_TURN_END)
        s_state = "TURN END";
    else if (current_state == ROBOT_STATE_CRASH)
        s_state = "CRASH";

    logWrite("Mode="+s_mode+" State="+s_state);
}