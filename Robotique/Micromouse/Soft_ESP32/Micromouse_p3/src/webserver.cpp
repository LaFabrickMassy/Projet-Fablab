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
#include "pid_calibration.h"
#include "webserver.h"
#include "simple_run.h"
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


//********************************************************************
// Initialize WiFi
//********************************************************************
void initWiFi() {
    #ifdef WIFI_MODE_AP
    if (WiFi.softAP(ssid, password))
    {
		Serial.println("*******************************");	
        Serial.println("Wifi AP launched");
        if (!WiFi.softAPConfig(local_ip, gateway, subnet))
            Serial.println("AP Config Failed");
        else
			Serial.print("IP Address: ");
            Serial.println(WiFi.localIP()); 
    }
    else
        Serial.println("Wifi AP failed");
    #else	// WIFI station
	WiFi.begin(ssid, password);
	Serial.print("Connecting to WiFi <");
	Serial.print(ssid);
	Serial.print("> with password <");
	Serial.print(password);
	Serial.println(">");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(1000);
	}
	Serial.print("Connected, IP=");
	Serial.print(WiFi.localIP());	
	Serial.print(" with signal stength ");
	Serial.println(WiFi.RSSI());
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
void cleanupClients()
{
    ws.cleanupClients();
}

//********************************************************************
// 
//********************************************************************
String getPIDCalParameters() {
    String jsonString;

    jsonString =  "{";
    jsonString += "\"kp\":"+String(pid_kp)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":"+String(pid_ki)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":"+String(pid_ki)+"\"";
    jsonString += ",";

    jsonString += "}";
    return jsonString;
}

//********************************************************************
// 
//********************************************************************
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        message = (char*)data;
        Serial.print("Received:<");
        Serial.print(message);
        Serial.println(">");
        // index **************************************
        if (message == "IDX_SR") {
            // set mode to simple run
            current_mode = ROBOT_MODE_SIMPLE_RUN;
            Serial.println("Mode simple run");
            initSimpleRun();
        }
        else if (message == "IDX_PC") {
            // set mode toPID calibration
            current_mode = ROBOT_MODE_PID_CAL;
            //Serial.println("Mode PID Calibration");
            initPIDRun();
        }

        // Simple run ********************************
        if ( current_mode == ROBOT_MODE_SIMPLE_RUN)
        {
            Serial.println("Message mode simple run");
            if (message == "SR_index") {
                // Set mode to stop
                stopSimpleRun();
                //Serial.println("Mode set to stop");
            }
            if (message == "SR_su") {
                if(speed < SPEED_MIN)
                    speed = SPEED_MIN;
                else
                    speed = speed * SPEED_FACTOR;
                
                if(speed > 1.)
                    speed = 1.;
            }
            if (message == "SR_sd") {
                speed = speed / SPEED_FACTOR;
                if(speed < SPEED_MIN)
                    speed = 0.;
            }    
            if (message == "SR_tl") {
                if (turn <= (1.-TURN_STEP))
                    turn += TURN_STEP;
            }
            if (message == "SR_tr") {
                if (turn >= (-1.+TURN_STEP))
                    turn -= TURN_STEP;
            }
            if (message == "SR_stop") {
                stopSimpleRun();
            }
            if (message == "SR_reset") {
                resetSimpleRun();
            }
            if (message == "SR_home") {
                // TODO
                ;
            }
            notifyClients(getRobotStatus());
        } // Simple run commands

        // PID calibration *****************************
        if ( current_mode == ROBOT_MODE_PID_CAL)
        {
            Serial.println("Message mode PID cal");
            if (message == "PC_index") {
                // Set mode to stop
                stopPIDRun();
                current_mode = ROBOT_MODE_STOP;
                //Serial.println("Mode set to stop");
            }
            if (message == "PC_start") {
                // start PID run
                startPIDRun();
            }
            if (message == "PC_stop") {
                // stop PID run
                stopPIDRun();
            }
            if (message == "PC_kp_up") {
                // 
                //Serial.println("kp_up");
                if (pid_kp > 0)
                    pid_kp = pid_kp * PID_STEP;
                else if (pid_kp == 0)
                    pid_kp = PID_MIN;
                else if (pid_kp >= -PID_MIN)
                    pid_kp = 0;
                else
                    pid_kp = pid_kp / PID_STEP;
                //Serial.println(pid_kp);
            }
            if (message == "PC_ki_up") {
                // 
                //Serial.println("ki_up");
                if (pid_ki > 0)
                    pid_ki = pid_ki * PID_STEP;
                else if (pid_ki == 0)
                    pid_ki = PID_MIN;
                else if (pid_ki >= -PID_MIN)
                    pid_ki = 0;
                else
                    pid_ki = pid_ki / PID_STEP;
                //Serial.println(pid_ki);
            }
            if (message == "PC_kd_up") {
                // 
                //Serial.println("kd_up");
                if (pid_kd > 0)
                    pid_kd = pid_kd * PID_STEP;
                else if (pid_kd == 0)
                    pid_kd = PID_MIN;
                else if (pid_kd >= -PID_MIN)
                    pid_kd = 0;
                else
                    pid_kd = pid_kd / PID_STEP;
                //Serial.println(pid_kd);
            }
            if (message == "PC_kp_dn") {
                // 
                //Serial.println("kp_dn");
                if (pid_kp < 0)
                    pid_kp = pid_kp * PID_STEP;
                else if (pid_kp == 0)
                    pid_kp = -PID_MIN;
                else if (pid_kp <= PID_MIN)
                    pid_kp = 0;
                else
                    pid_kp = pid_kp / PID_STEP;
                //Serial.println(pid_kp);
            }
            if (message == "PC_ki_dn") {
                // 
                //Serial.println("ki_dn");
                if (pid_ki < 0)
                    pid_ki = pid_ki * PID_STEP;
                else if (pid_ki == 0)
                    pid_ki = -PID_MIN;
                else if (pid_ki <= PID_MIN)
                    pid_ki = 0;
                else
                    pid_ki = pid_ki / PID_STEP;
                //Serial.println(pid_ki);
            }
            if (message == "PC_kd_dn") {
                // 
                //Serial.println("kd_dn");
                if (pid_kd < 0)
                    pid_kd = pid_kd * PID_STEP;
                else if (pid_kd == 0)
                    pid_kd = -PID_MIN;
                else if (pid_kd <= PID_MIN)
                    pid_kd = 0;
                else
                    pid_kd = pid_kd / PID_STEP;
                //Serial.println(pid_kd);
            }
            notifyClients(getPIDStatus());
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


