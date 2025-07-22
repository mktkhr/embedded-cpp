#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <Arduino.h>

String generateHtml(const char *macAddress, const char *ssid, const char *version, const char *bootMode);

extern const char INDEX_HTML[] PROGMEM;

#endif