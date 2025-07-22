#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>

bool connectToAccessPoint(const char *ssid, const char *password);
void postRequest(String host, String uri, String measureData);
void getRequest();

#endif