#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SPIFFS.h>
#include "Wifi.h"
#include "parameters.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "distance_sensor.h"
#include "pid.h"
#include "parameters_calibration.h"
#include "robot_controller.h" 
#include "webserver.h"
#include "test_drive.h"
#include "logutils.h"
#include "micromouse.h"

//Json Variable to Hold Slider Values
JSONVar RobotStatus;

/* Put IP Address details */
IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");

String message = "";

#define PC_SELPARAMMODE_KP 0
#define PC_SELPARAMMODE_KI 1
#define PC_SELPARAMMODE_KD 2

int pcSelParamMode = PC_SELPARAMMODE_KP;

//********************************************************************
// Initialize WiFi
//********************************************************************
void initWiFi() {
    #ifdef WIFI_MODE_AP
    if (WiFi.softAP(ssid, password))
    {
		logWrite("*******************************");	
        logWrite("Wifi AP launched");
        if (!WiFi.softAPConfig(local_ip, gateway, subnet))
            logWrite("AP Config Failed");
        else
			logWrite("IP Address: ");
            logWrite(String(WiFi.localIP().toString())); 
    }
    else
        logWrite("Wifi AP failed");
    #else	// WIFI station
	WiFi.begin(ssid, password);
	logWrite("Connecting to WiFi \'");
	logWrite(ssid);
	logWrite("\' with password \'");
	logWrite(password);
	logWrite("\'");
	while (WiFi.status() != WL_CONNECTED) {
		logWrite('.');
		delay(1000);
	}
	logWrite("Connected, IP=");
	logWrite(WiFi.localIP());	
	logWrite(" with signal stength ");
	logWrite(WiFi.RSSI());
    #endif
}

//********************************************************************
// 
//********************************************************************
void initWebServer() {
    // Start server
    AsyncElegantOTA.begin(&server);
    server.begin();

    initWebSocket();
  
    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });
  
    server.serveStatic("/", SPIFFS, "/");
    logWrite("Webserver launched");
}

//********************************************************************
// 
//********************************************************************
void notifyClients(String message) {
    ws.textAll(message);
}

//********************************************************************
// 
//********************************************************************
void cleanupClients() {
    ws.cleanupClients();
}

//********************************************************************
// 
//********************************************************************
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        message = (char*)data;
        logWrite("WS: msg=\'" + message + "\'");

        if (message == "") {
            // emty messae, do nothing
            ;
        }
        // index -> Test Drive ****************************
        else if (message == "IDX_TD") {
            // set mode to simple run
            current_mode = ROBOT_MODE_TEST_DRIVE;
            current_state = ROBOT_STATE_STOP;
            testDriveInit();
            #if TRACE_LEVEL >= 2
            logWrite("WS: testDriveInit()");
            logRobotState();
            #endif
        }

        // index -> Parameter Calibration *****************
        else if (message == "IDX_PC") {
            // set mode toPID calibration
            current_mode = ROBOT_MODE_PARAM;
            current_state = ROBOT_STATE_STOP;
            #if TRACE_LEVEL >= 2
            logWrite("WS: IDX_PC -> ParamCalInit()");
            #endif
            ParamCalInit();
            #if TRACE_LEVEL >= 2
            logWrite("WS: ParamCalInit()");
            #endif
        }
 
        // Simple run ********************************
        else if ( current_mode == ROBOT_MODE_TEST_DRIVE ) {
            logWrite("WS: Mode TEST_DRIVE");
            if (message == "TD_index") {
                // Set mode to stop
                #if TRACE_LEVEL >= 2
                logWrite("WS: Index, testDriveStop() === START");
                logRobotState();
                #endif
                testDriveStop();
                #if TRACE_LEVEL >= 2
                logWrite("WS: testDriveStop() === DONE");
                logRobotState();
                #endif
            }
            else if (message == "TD_su") {
                if(speed < SPEED_MIN)
                    speed = SPEED_MIN;
                else
                    speed = speed * SPEED_FACTOR;
                
                if(speed > 1.)
                    speed = 1.;
            }
            else if (message == "TD_sd") {
                speed = speed / SPEED_FACTOR;
                if(speed < SPEED_MIN)
                    speed = 0.;
            }    
            else if (message == "TD_tl") {
                if (turn <= (1.-TURN_STEP))
                    turn += TURN_STEP;
            }
            else if (message == "TD_tr") {
                if (turn >= (-1.+TURN_STEP))
                    turn -= TURN_STEP;
            }
            else if (message == "TD_stop") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: TD_Stop -> testDriveStop() === START");
                logRobotState();
                #endif
                testDriveStop();
                #if TRACE_LEVEL >= 2
                logWrite("WS: testDriveStop() ended");
                logRobotState();
                #endif
            }
            else if (message == "TD_run") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: TD_Run -> testDriveRun()");
                logRobotState();
                #endif
                testDriveRunInit();
                #if TRACE_LEVEL >= 2
                logWrite("WS: testDriveRun() ended");
                logRobotState();
                #endif
            }
            else if (message == "TD_reset") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: TD_reset -> testDriveInit()");
                logRobotState();
                #endif
                testDriveInit();
                #if TRACE_LEVEL >= 2
                logWrite("WS: testDriveInit() ended");
                logRobotState();
                #endif
            }
            else if (message == "TD_rccw") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: TD_rccw -> testDriveRotateInitCCW()");
                logRobotState();
                #endif
                testDriveRotateInitCCW();
                #if TRACE_LEVEL >= 2
                logWrite("WS: testDriveRotateInitCCW() ended");
                logRobotState();
                #endif
            }
            else if (message == "TD_rcw") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: TD_rcw -> testDriveRotateInitCW()");
                logRobotState();
                #endif
                testDriveRotateInitCW();
                #if TRACE_LEVEL >= 2
                logWrite("WS: testDriveRotateInitCW() ended");
                logRobotState();
                #endif
            }
            else if (message == "TD_home") {
                // TODO
                ;
            }
            else {
                logWrite("WS: command \'"+message+"\' not available in TD mode");
            }
            notifyClients(getRobotStatus());
        } // Simple run commands

        // PID calibration *****************************
        else if ( current_mode == ROBOT_MODE_PARAM) {
            logWrite("WS: Mode PARAM");
            if (message == "PC_index") {
                // Set mode to stop
                ParamCalStop();
                #if TRACE_LEVEL >= 2
                logWrite("WS: PC Index, ParamCalStop()");
                logRobotState();
                #endif
            }
            else if (message == "PC_start") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: ParamCalRunInit() === START");
                logRobotState();
                #endif
                // start PID run
                ParamCalRunInit();
                #if TRACE_LEVEL >= 2
                logWrite("WS: ParamCalRunInit() === DONE");
                logRobotState();
                #endif
            }
            else if (message == "PC_uturn") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: ParamCalRotateInit() === START");
                logRobotState();
                #endif
                // start PID run
                ParamCalRotateInit();
                #if TRACE_LEVEL >= 2
                logWrite("WS: ParamCalRotateInit() === DONE");
                logRobotState();
                #endif
            }
            else if (message == "PC_stop") {
                #if TRACE_LEVEL >= 2
                logWrite("WS: ParamCalStop() === START");
                logRobotState();
                #endif
                // stop PID run
                ParamCalStop();
                #if TRACE_LEVEL >= 2
                logWrite("WS: ParamCalStop() === DONE");
                logRobotState();
                #endif
            }
            else if (message == "PC_auto_on") {
                // stop PID run
                pcFlagAuto = 1;
                #if TRACE_LEVEL >= 2
                logWrite("WS: PC flag_auto set to "+String(pcFlagAuto)+" =======");
                logRobotState();
                #endif
            }
            else if (message == "PC_auto_off") {
                // stop PID run
                pcFlagAuto = 0;
                #if TRACE_LEVEL >= 2
                logWrite("WS: PC flag_auto set to "+String(pcFlagAuto)+" =======");
                logRobotState();
                #endif
            }
            else if (message == "PC_sel_kp") {
                pcSelParamMode = PC_SELPARAMMODE_KP;
            }
            else if (message == "PC_sel_ki") {
                pcSelParamMode = PC_SELPARAMMODE_KI;
            }
            else if (message == "PC_sel_kd") {
                pcSelParamMode = PC_SELPARAMMODE_KD;
            }
            else if (message == "PC_+3") {
                if (pcSelParamMode == PC_SELPARAMMODE_KD)
                    pidSensors_kd += 0.01;
                else if (pcSelParamMode == PC_SELPARAMMODE_KI)
                    pidSensors_ki += 0.01;
                else
                    pidSensors_kp += 0.01;
                notifyClients(getPIDStatus());            
            }
            else if (message == "PC_+2") {
                if (pcSelParamMode == PC_SELPARAMMODE_KD)
                    pidSensors_kd += 0.001;
                else if (pcSelParamMode == PC_SELPARAMMODE_KI)
                    pidSensors_ki += 0.001;
                else
                    pidSensors_kp += 0.001;
                notifyClients(getPIDStatus());            
            }
            else if (message == "PC_+1") {
                if (pcSelParamMode == PC_SELPARAMMODE_KD)
                    pidSensors_kd += 0.0001;
                else if (pcSelParamMode == PC_SELPARAMMODE_KI)
                    pidSensors_ki += 0.0001;
                else
                    pidSensors_kp += 0.0001;
                notifyClients(getPIDStatus());            
            }
            else if (message == "PC_-3") {
                if (pcSelParamMode == PC_SELPARAMMODE_KD)
                    pidSensors_kd -= 0.01;
                else if (pcSelParamMode == PC_SELPARAMMODE_KI)
                    pidSensors_ki -= 0.01;
                else
                    pidSensors_kp -= 0.01;
                notifyClients(getPIDStatus());            
            }
            else if (message == "PC_-2") {
                if (pcSelParamMode == PC_SELPARAMMODE_KD)
                    pidSensors_kd -= 0.001;
                else if (pcSelParamMode == PC_SELPARAMMODE_KI)
                    pidSensors_ki -= 0.001;
                else
                    pidSensors_kp -= 0.001;
                notifyClients(getPIDStatus());            
            }
            else if (message == "PC_-1") {
                if (pcSelParamMode == PC_SELPARAMMODE_KD)
                    pidSensors_kd -= 0.0001;
                else if (pcSelParamMode == PC_SELPARAMMODE_KI)
                    pidSensors_ki -= 0.0001;
                else
                    pidSensors_kp -= 0.0001;
                notifyClients(getPIDStatus());            
            }
            else {
                logWrite("WS: command \'"+message+"\' available only in PC mode");
            }
        }        
        
        cleanupClients();
    }
}

//********************************************************************
// 
//********************************************************************
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            logWrite("WebSocket client " + String(client->id()) + " connected from " + client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

//********************************************************************
// 
//********************************************************************
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


