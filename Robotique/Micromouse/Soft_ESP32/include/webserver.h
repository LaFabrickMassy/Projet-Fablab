#ifndef _WEBSERVER_H_

#define _WEBSERVER_H_

#define PIDSENSORS_MIN 0.001
#define PIDSENSORS_STEP 1.2 // Step for up/down commands

void initWiFi();
void initWebServer();
void notifyClients(String message);
void cleanupClients();
String getPIDCalParameters();
void initWebSocket();

#endif
