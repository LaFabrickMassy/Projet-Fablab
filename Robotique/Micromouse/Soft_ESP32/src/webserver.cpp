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

//* Web server definitions ********************************
const char* ap_ssid = "ESP32";
const char* ap_password = NULL;
const char* st_ssid = "HUAWEI_E5576_3910";
const char* st_password = "D3HBJ7RB0L8";

#define CHECK_WIFI_AP 1

String message;

//********************************************************************
// Initialize WiFi
//********************************************************************
void initWiFi() {
    bool station_connected;

    station_connected = false;

    // Check if expected WIFI network is present, else open the AP
    #if CHECK_WIFI_AP
    bool ap_found;
    int nb_wifi_networks;
    int i;
    WiFi.mode(WIFI_STA);
    nb_wifi_networks = WiFi.scanNetworks();
    ap_found = false;
    if (nb_wifi_networks > 0) {
        for (i=0;i<nb_wifi_networks;i++){
            if (WiFi.SSID(i) == st_ssid) {
                // The SSID is opened
                ap_found = true;
                WiFi.scanDelete();
                break;
            }
        }
    }
    if (ap_found == true) {
        WiFi.begin(st_ssid, st_password);
        Serial.println(String("Trying to connect to SSID <")+st_ssid+String("> "));
        i = 10;
        while(WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
            delay(500);
            if (i<=0) { // tried during more than 5 sec.
                break;
            }
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("connected");
            Serial.print("Connected, IP=");
            Serial.println(WiFi.localIP().toString());	
            Serial.print("with signal stength ");
            Serial.println(String(WiFi.RSSI()));

            station_connected = true;
        }
    }
    #endif

    // if not connected, open the AP
    if(station_connected == false) {
        WiFi.mode(WIFI_AP);

        if (!WiFi.softAPConfig(local_ip, gateway, subnet)) {
            Serial.println("AP Config Failed");
        }
        else {
            if (WiFi.softAP(ap_ssid, ap_password, WIFI_CHANNEL)) {
                Serial.println("*******************************");	
                Serial.println("Wifi AP launched");
                    Serial.println("IP Address: ");
                    Serial.println(String(WiFi.softAPIP().toString())); 
            }
            else {
                Serial.println("Wifi AP failed");
            }
        }
    }
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


