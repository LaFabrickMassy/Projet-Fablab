
#include "motor.h"

//Json Variable to Hold Slider Values
JSONVar RobotStatus;


// Initialize WiFi
void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void notifyClients(String message) {
    ws.textAll(message);
}

//Get Slider Values
String getRobotStatus(){
    String jsonString;
    
    jsonString = "{";
    jsonString += "\"speed\":\""+String(speed*100)+"\"";
    jsonString += ",";
    jsonString += "\"turn\":\""+String(turn*100)+"\"";
    jsonString += ",";
    jsonString += "\"x\":\""+String(pos_x)+"\"";
    jsonString += ",";
    jsonString += "\"y\":\""+String(pos_y)+"\"";
    jsonString += ",";
    jsonString += "\"h\":\""+String(heading)+"\"";
    jsonString += ",";
    if (motorL.flag_reverse)
        jsonString += "\"eL\":\""+String(-encoderL.count)+"\"";
    else
        jsonString += "\"eL\":\""+String(encoderL.count)+"\"";
    jsonString += ",";
    if (motorR  .flag_reverse)
        jsonString += "\"eR\":\""+String(-encoderR.count)+"\"";
    else
        jsonString += "\"eR\":\""+String(encoderR.count)+"\"";
    jsonString += ",";
    jsonString += "\"sL\":\""+String(distance_sensor1())+"\"";
    jsonString += ",";
    jsonString += "\"sSL\":\""+String(distance_sensor2())+"\"";
    jsonString += ",";
    jsonString += "\"sF\":\""+String(distance_sensor3())+"\"";
    jsonString += ",";
    jsonString += "\"sSR\":\""+String(distance_sensor4())+"\"";
    jsonString += ",";
    jsonString += "\"sR\":\""+String(distance_sensor5())+"\"";
    jsonString += "}";
    
    return jsonString;
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        message = (char*)data;
        Serial.print("Received:<");
        Serial.println(message);
        if (message.indexOf("bup") >= 0) {
            cmd_speedup();
            notifyClients(getRobotStatus());
        }
        if (message.indexOf("bdn") >= 0) {
            cmd_speeddown();
            notifyClients(getRobotStatus());
        }    
        if (message.indexOf("blt") >= 0) {
            cmd_turnleft();
            notifyClients(getRobotStatus());
        }
        if (message.indexOf("brt") >= 0) {
            cmd_turnright();
            notifyClients(getRobotStatus());
        }
        if (message.indexOf("bst") >= 0) {
            cmd_stop();
            notifyClients(getRobotStatus());
        }
        if (message.indexOf("bst") >= 0) {
            cmd_home();
            notifyClients(getRobotStatus());
        }
        if (strcmp((char*)data, "getValues") == 0) {
            notifyClients(getRobotStatus());
        }
    }
}

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

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


