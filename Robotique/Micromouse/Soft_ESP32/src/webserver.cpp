#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SPIFFS.h>
#include "Wifi.h"
#include "parameters.h"
//#include "motor.h"
//#include "wheel_encoder.h"
//#include "distance_sensor.h"
//#include "pid.h"
//#include "parameters_calibration.h"
//#include "robot_controller.h" 
//#include "robot_state.h"
#include "webserver.h"
//#include "test_drive.h"
#include "logutils.h"
//#include "robot.h"
//#include "maze.h"
#include "hmi.h"
#include "micromouse.h"

//Json Variable to Hold Slider Values
JSONVar RobotStatus;

/* Put IP Address details */
#define WIFI_CHANNEL 8
IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");

String message;

//********************************************************************
// Initialize WiFi
//********************************************************************
void initWiFi() {
    #ifdef WIFI_MODE_AP
    if (WiFi.softAP(ssid, password, WIFI_CHANNEL))
    {
		Serial.println("*******************************");	
        Serial.println("Wifi AP launched");
        if (!WiFi.softAPConfig(local_ip, gateway, subnet))
            Serial.println("AP Config Failed");
        else
			Serial.println("IP Address: ");
            Serial.println(String(WiFi.localIP().toString())); 
    }
    else
        Serial.println("Wifi AP failed");
    #else	// WIFI station
	WiFi.begin(ssid, password);
	Serial.println("Connecting to WiFi \'");
	Serial.println(ssid);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.println(String("."));
		delay(1000);
	}
	Serial.println("Connected, IP=");
	Serial.println(String(WiFi.localIP()));	
	Serial.println(" with signal stength ");
	Serial.println(String(WiFi.RSSI()));
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
        logWrite("WS: rcvd msg=\'" + String(message) + "\'");

        if (message == "") {
            // emty message, do nothing
            ;
        }
        else {
            hmiHandleWSMessage(message);
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


