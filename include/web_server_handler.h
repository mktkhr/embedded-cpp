#ifndef WEB_SERVER_HANDLER_H
#define WEB_SERVER_HANDLER_H

#include <Arduino.h>
#include <WebServer.h>

extern WebServer Server;

void handleRoot();
void handleNotFound();

#endif