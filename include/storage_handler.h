#ifndef STORAGE_HANDLER_H
#define STORAGE_HANDLER_H

#include <Arduino.h>
#include <Preferences.h>

extern Preferences preferences;
extern const char *emsPreference;
extern const char *NORMAL_MODE;
extern const char *MAINTENANCE_MODE;

void writePreference(int target, String value);
String readPreference(int target);

void listDir(const char *dirname, uint8_t levels);
void createDir(const char *path);
void removeDir(const char *path);
void readFile(const char *path);
void writeFile(const char *path, String message);
void appendFile(const char *path, String message);
void renameFile(const char *path1, const char *path2);
String deleteFile(const char *path);
String readSdSize();

#endif