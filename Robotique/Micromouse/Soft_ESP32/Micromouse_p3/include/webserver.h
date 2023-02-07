#ifndef _WEBSERVER_H_

#define _WEBSERVER_H_

void initWiFi();
void initWebServer();
void notifyClients(String message);
void cleanupClients();
String getPIDCalParameters();
void initWebSocket();

#endif
