#include <Arduino.h>
#include "SPIFFS.h"
#include "motor.h"
#include "webserver.h"
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

