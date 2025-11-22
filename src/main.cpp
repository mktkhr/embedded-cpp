#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SDI12.h>
#include "M5_ENV.h"
#include "Adafruit_SGP30.h"
#include <M5_DLight.h>
#include "SD.h"
#include "SPI.h"
#include "FS.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include <FastLED.h>
#include "index_html.h"
#include "version.h"
#include "sensor_handler.h"
#include "wifi_handler.h"
#include "storage_handler.h"
#include "web_server_handler.h"

#define SD_PIN 0
#define DATA_PIN 19
#define ADC_PIN 32
#define ANALOG_PIN 33
#define ADC_CHANNEL ADC_CHANNEL_4
#define NUM_LED 1
#define LED_PIN 27
#define BUTTON_PIN 39

CRGB led[NUM_LED];

uint8_t sda = 21;
uint8_t scl = 22;

String emsHost = "www.ems-engineering.jp";
int productionPort = 443;
int localPort = 8080;
String postMeasuredDataUri = "/api/ems/measured-data";
String getMicroControllerUri = "/api/ems/micro-controller/detail/no-session";

int measurementInterval;
String measurementTargetSensorAddress;
int calculateNumber = 100;
bool sdFlag = false;
float temperature, humidity;
struct tm timeInfo;
char timeData[20];
int maxSdi12SensorNum = 4;

int bootMode = 0;
WebServer Server(80);
const char *ssid_server = "ems_stamp";
const char *pass_server = "00000000";
IPAddress ip(192, 168, 4, 11);
IPAddress subnet(255, 255, 255, 0);

Preferences preferences;
const char *emsPreference = "emsPreference";
const char *NORMAL_MODE = "通常モード";
const char *MAINTENANCE_MODE = "メンテナンスモード";

SDI12 mySDI12(DATA_PIN);
SHT3X sht30;
QMP6988 qmp6988;
Adafruit_SGP30 sgp;
esp_adc_cal_characteristics_t adcChar;
M5_DLight bh1750;

void setup()
{
  // Brownout検出を無効化 (電源安定化のため)
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  while (!Serial)
  {
  }

  Wire.begin(sda, scl);
  delay(100);  // I2Cバス安定化待ち

  bh1750.begin(&Wire, sda, scl);
  bh1750.setMode(CONTINUOUSLY_H_RESOLUTION_MODE);

  qmp6988.init();
  mySDI12.begin();

  FastLED.addLeds<SK6812, LED_PIN, RGB>(led, NUM_LED);
  FastLED.setBrightness(1);

  pinMode(BUTTON_PIN, INPUT);
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW)
  {
    bootMode = 1;
    Serial.println("Starting web server.....");
    delay(2000);  // 電源安定化待ち

    // WiFi起動前に一度電源を安定させる
    led[0] = 0x000000;
    FastLED.show();
    delay(500);

    WiFi.mode(WIFI_AP);
    delay(500);

    // WiFi送信パワーを下げて電力消費を抑える
    WiFi.setTxPower(WIFI_POWER_11dBm);  // 最低パワー

    WiFi.softAPConfig(ip, ip, subnet);
    delay(500);
    if (WiFi.softAP(ssid_server, pass_server))
    {
      led[0] = 0x0000ff;
      FastLED.show();
    }
    else
    {
      led[0] = 0x00f000;
      FastLED.show();
      delay(5000);
      led[0] = 0x000000;
      FastLED.show();
    }
    delay(100);
    Serial.print("SSID: ");
    Serial.println(ssid_server);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    Server.on("/", handleRoot);
    Server.onNotFound(handleNotFound);
    Server.begin();
  }
  while (bootMode == 1)
  {
    Server.handleClient();
  }

  for (int i = 0; i < 3; i++)
  {
    if (SD.begin(SD_PIN))
    {
      sdFlag = true;
      break;
    }
  }

  if (sdFlag == true)
  {
    Serial.println("SD card Connection done");
    File file = SD.open("/log.csv");
    if (!file)
    {
      String first_row = "Timestamp,Serial number,# of SDI-12 sensor,Address,Volumetric water content,Soil temperature,Bulk relative permittivity,Soil bulk electric conductivity(TDT),Soil bulk electric conductivity(TDR),Soil pore water electric coductivity,Gravitational acceleration(x-axis),Gravitational accelaration(y-axis),Gravitational accelaration(z-axis),Address,Volumetric water content,Soil temperature,Bulk relative permittivity,Soil bulk electric conductivity(TDT),Soil bulk electric conductivity(TDR),Soil pore water coductivity,Gravitational acceleration(x-axis),Gravitational acceleration(y-axis),Gravitational acceleration(z-axis),Address,Volumetric water content,Soil temperature,Bulk relative permittivity,Soil bulk electric conductivity(TDT),Soil bulk electric conductivity(TDR),Soil pore water coductivity,Gravitational acceleration(x-axis),Gravitational acceleration(y-axis),Gravitational acceleration(z-axis),Address,Volumetric water content,Soil temperature,Bulk relative permittivity,Soil bulk electric conductivity(TDT),Soil bulk electric conductivity(TDR),Soil pore water coductivity,Gravitational acceleration(x-axis),Gravitational acceleration(y-axis),Gravitational acceleration(z-axis),Atmospheric pressure,Temperature,Humidity,CO2 concentration,Total volatile organic compounds,Illumination,Analog value,Voltage\n";
      String second_row = "(YYYY/MM/DD hh:mm:ss),(-),(-),(-),(%),(C),(-),(dS/m),(uS/cm),(uS/cm),(G),(G),(G),(-),(%),(C),(-),(dS/m),(uS/cm),(uS/cm),(G),(G),(G),(-),(%),(C),(-),(dS/m),(uS/cm),(μS/cm),(G),(G),(G),(-),(%),(C),(-),(dS/m),(uS/cm),(uS/cm),(G),(G),(G),(hPa),(C),(%),(ppm),(ppb),(lux),(-),(V)\n";
      appendFile("/log.csv", first_row);
      appendFile("/log.csv", second_row);
    }
  }
  else
  {
    Serial.println("Card Mount Failed");
  }

  if (!sgp.begin())
  {
    Serial.println("Sensor not found");
  }
  else
  {
    Serial.print("Found SGP30 serial #");
    Serial.println(sgp.serialnumber[0], HEX);
  }

  pinMode(ANALOG_PIN, INPUT);
  pinMode(ADC_PIN, ANALOG);
  adcInit();

  String ssidRead = readPreference(1);
  String passwordRead = readPreference(2);
  char ssid[30];
  char password[30];
  ssidRead.toCharArray(ssid, ssidRead.length() + 1);
  passwordRead.toCharArray(password, passwordRead.length() + 1);
  const bool wifiStatus = connectToAccessPoint(ssid, password);
  if (!wifiStatus)
  {
    led[0] = 0xff0000;
    FastLED.show();
    delay(5000);
    led[0] = 0x000000;
    FastLED.show();
    ESP.restart();
  }

  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com");
  getRequest();
  measurementInterval = readPreference(4).toInt();
  measurementTargetSensorAddress = readPreference(5);
}

void loop()
{
  getLocalTime(&timeInfo);
  if ((timeInfo.tm_min % measurementInterval == 0) && (timeInfo.tm_sec == 0))
  {
    sprintf(timeData, "%04d/%02d/%02d %02d:%02d:%02d", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    Serial.println(timeData);
    String macAddress = WiFi.macAddress();

    String postString = "{\"macAddress\":\"" + WiFi.macAddress() + "\",";
    String sdSaveData = "";

    String adcResult = readAdcValue(calculateNumber);
    postString += "\"voltage\":\"" + adcResult + "\",";

    Serial.println("測定間隔:" + String(measurementInterval));
    Serial.println("測定対象アドレス: " + measurementTargetSensorAddress);

    int sensorNum;
    int addressLength = measurementTargetSensorAddress.length();
    if (addressLength == 0)
    {
      sensorNum = 0;
    }
    else
    {
      sensorNum = (addressLength / 2) + 1;
    }

    String measurementTargetAddressArray[sensorNum];
    split(measurementTargetSensorAddress, measurementTargetAddressArray);

    String sdiResultForSd = "";
    String sdiResultForPost = "";
    postString += "\"sdi12Param\": [";
    for (int i = 0; i < sensorNum; i++)
    {
      String sdi12Response = measureSdi12(measurementTargetAddressArray[i]);
      sdiResultForSd += "," + convertSdi12ResultForSd(sdi12Response);
      sdiResultForPost += convertSdi12ResultForPost(sdi12Response);
      if (i != sensorNum - 1 && sdiResultForPost != "")
      {
        sdiResultForPost += ",";
      }
    }
    if (maxSdi12SensorNum > sensorNum)
    {
      for (int i = 0; i < (maxSdi12SensorNum - sensorNum); i++)
      {
        sdiResultForSd += ",,,,,,,,,,";
      }
    }

    postString += sdiResultForPost + "],";
    postString += "\"environmentalDataParam\":[{";

    String env3Result = measureEnv3();
    postString += convertEnv3ResultForPost(env3Result);

    String sgp30Result = measureSgp30();
    postString += convertSgp30ResultForPost(sgp30Result);

    String illuminationResult = measureIllumination();
    if (illuminationResult != "")
    {
      postString += "\"light\":\"" + illuminationResult + "\",";
    }

    String analogResult = readAnalogValue();
    if (illuminationResult != "")
    {
      postString += "\"analogValue\":\"" + analogResult + "\"";
    }

    postString += "}]}";
    Serial.println(postString);

    Serial.println(sdiResultForSd);

    sdSaveData += String(timeData) + "," + macAddress + "," + sensorNum + "," + sdiResultForSd + convertEnv3ResultForSd(env3Result) + convertSgp30ResultForSd(sgp30Result) + illuminationResult + "," + analogResult + "," + adcResult + "\n";
    Serial.println(sdSaveData);
    appendFile("/log.csv", sdSaveData);
    postRequest(emsHost, postMeasuredDataUri, postString);
    delay(1000);
  }
}
