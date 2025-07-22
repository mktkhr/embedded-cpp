#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <Arduino.h>
#include <SDI12.h>
#include "M5_ENV.h"
#include "Adafruit_SGP30.h"
#include <M5_DLight.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"

extern SDI12 mySDI12;
extern SHT3X sht30;
extern QMP6988 qmp6988;
extern Adafruit_SGP30 sgp;
extern esp_adc_cal_characteristics_t adcChar;
extern M5_DLight bh1750;
extern float temperature, humidity;

void adcInit(void);
String readAdcValue(int calculateNumber);
String readAnalogValue();
String measureEnv3();
String measureSgp30();
String measureIllumination();
String measureSdi12(String sensorAddress);
String sendCommandAndCollectResponse(String myCommand, int sendInterval, int requestNumber);
boolean checkActiveSdi12(byte i);
boolean addressChange(String present_address, String following_address);
uint32_t getAbsoluteHumidity(float temperature, float humidity);

void split(String data, String *dataArray);
String convertEnv3ResultForSd(String result);
String convertEnv3ResultForPost(String result);
String convertSgp30ResultForSd(String result);
String convertSgp30ResultForPost(String result);
String convertSdi12ResultForSd(String result);
String convertSdi12ResultForPost(String result);

#endif