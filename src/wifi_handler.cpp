#include "wifi_handler.h"
#include "storage_handler.h"
#include <ArduinoJson.h>
#include <WiFi.h>

extern String emsHost;
extern int productionPort;
extern int localPort;
extern String getMicroControllerUri;
extern const char *NORMAL_MODE;
extern const char *MAINTENANCE_MODE;

bool connectToAccessPoint(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  int waitCountInSec = 0;
  while (WiFi.status() != WL_CONNECTED) {
    waitCountInSec++;
    delay(1000);
    Serial.print(".");
    if (waitCountInSec == 10) {
      Serial.println("Wi-Fi Connection failed. Please check the ssid and password.");
      return false;
    }
  }
  Serial.println("WiFi connected.");
  return true;
}

void postRequest(String host, String uri, String measureData) {
  Serial.println("\r\n-----Connecting to HOST-----\r\n");
  String targetHost;
  int targetPort;
  WiFiClientSecure sslClient;
  WiFiClient client;

  const String bootMode = readPreference(0);
  if (bootMode == String(NORMAL_MODE)) {
    targetHost = emsHost;
    targetPort = productionPort;
    sslClient.setInsecure();
  } else {
    targetHost = readPreference(3);
    targetPort = localPort;
  }

  Serial.print("TargetHost: ");
  Serial.println(targetHost);
  Serial.print("TargetPort: ");
  Serial.println(targetPort);

  int length = targetHost.length() + 1;
  char bufferArray[length];
  targetHost.toCharArray(bufferArray, length);
  const char *hostCharPointer = bufferArray;

  const int requestLimit = 5;

  Serial.print("MeasuredData data:");
  Serial.println(measureData);

  for (int j = 0; j < requestLimit; j++) {
    if (bootMode == String(NORMAL_MODE)) {
      if (sslClient.connect(hostCharPointer, targetPort)) {
        Serial.println("\r\n-----Posting measured data-----\r\n");
        sslClient.println("POST " + uri + " HTTP/1.1");
        sslClient.println("Host: " + targetHost + ":" + String(targetPort));
        sslClient.println("Connection: close");
        sslClient.println("Content-Type: application/json");
        sslClient.print("Content-Length: ");
        sslClient.println(measureData.length());
        sslClient.println();
        sslClient.println(measureData);
        delay(100);
        String apiResponse = sslClient.readString();
        delay(100);
        Serial.print("API response:");
        Serial.println(apiResponse);
        sslClient.stop();
        return;
      } else {
        Serial.println(".");
        delay(200);
      }
    } else {
      if (client.connect(hostCharPointer, targetPort)) {
        Serial.println("\r\n-----Posting measured data-----\r\n");
        client.println("POST " + uri + " HTTP/1.1");
        client.println("Host: " + targetHost + ":" + String(targetPort));
        client.println("Connection: close");
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(measureData.length());
        client.println();
        client.println(measureData);
        delay(100);
        String apiResponse = client.readString();
        delay(100);
        Serial.print("API response:");
        Serial.println(apiResponse);
        client.stop();
        return;
      } else {
        Serial.println(".");
        delay(200);
      }
    }
  }
  Serial.println("\r\n-----POST request failed.-----\r\n");
}

void getRequest() {
  Serial.println("\r\n-----Connecting to HOST-----\r\n");
  String targetHost;
  int targetPort;
  WiFiClientSecure sslClient;
  WiFiClient client;
  const int BUFFER_SIZE = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(1);
  StaticJsonDocument<BUFFER_SIZE> jsonBuffer;

  const String bootMode = readPreference(0);
  if (bootMode == String(NORMAL_MODE)) {
    targetHost = emsHost;
    targetPort = productionPort;
    sslClient.setInsecure();
  } else {
    targetHost = readPreference(3);
    targetPort = localPort;
  }

  Serial.print("TargetHost: ");
  Serial.println(targetHost);
  Serial.print("TargetPort: ");
  Serial.println(targetPort);

  int length = targetHost.length() + 1;
  char bufferArray[length];
  targetHost.toCharArray(bufferArray, length);
  const char *hostCharPointer = bufferArray;

  const int requestLimit = 5;

  for (int j = 0; j < requestLimit; j++) {
    if (bootMode == String(NORMAL_MODE)) {
      if (sslClient.connect(hostCharPointer, targetPort)) {
        Serial.println("\r\n-----Get measuring settings-----\r\n");
        sslClient.println("GET " + getMicroControllerUri + "?macAddress=" + WiFi.macAddress() + " HTTP/1.1");
        sslClient.println("Host: " + targetHost + ":" + String(targetPort));
        sslClient.println("Connection: close");
        sslClient.println("Content-Type: application/json");
        sslClient.print("Content-Length: 0");
        sslClient.println();
        sslClient.println();
        delay(100);

        String response;
        while (sslClient.available()) {
          response = sslClient.readStringUntil('\r');
          response.trim();
          if (response.length() == 0) {
            break;
          }
        }
        delay(100);

        String responseBuffer;
        while (sslClient.available()) {
          response = sslClient.readStringUntil('\r');
          response.trim();
          responseBuffer.concat(response);
        }

        char json[responseBuffer.length() + 1];
        responseBuffer.toCharArray(json, sizeof(json));
        Serial.println(json);
        auto error = deserializeJson(jsonBuffer, json);
        delay(200);
        if (error) {
          Serial.print("JSON parse error: ");
          Serial.println(error.c_str());
          return;
        }
        const char *interval = jsonBuffer["interval"];
        Serial.println(interval);
        const char *sdi12Address = jsonBuffer["sdi12Address"];
        Serial.println(sdi12Address);

        String intervalString = String(interval);
        String sdi12AddressString = String(sdi12Address);
        if (intervalString == "1" || intervalString == "5" || intervalString == "10" || intervalString == "15" || intervalString == "20" || intervalString == "30" || intervalString == "60") {
          writePreference(4, intervalString);
        }

        writePreference(5, sdi12AddressString);

        Serial.println("closing connection");

        delay(100);
        sslClient.stop();
        return;
      } else {
        Serial.println(".");
        delay(200);
      }
    } else {
      if (client.connect(hostCharPointer, targetPort)) {
        Serial.println("\r\n-----Get measuring settings-----\r\n");
        client.println("GET " + getMicroControllerUri + "?macAddress=" + WiFi.macAddress() + " HTTP/1.1");
        client.println("Host: " + targetHost + ":" + String(targetPort));
        client.println("Connection: close");
        client.println("Content-Type: application/json");
        client.print("Content-Length: 0");
        client.println();
        client.println();
        delay(100);

        String response;
        while (client.available()) {
          response = client.readStringUntil('\r');
          Serial.print(response);
          response.trim();
          if (response.length() == 0) {
            break;
          }
        }
        delay(100);

        String responseBuffer;
        while (client.available()) {
          response = client.readStringUntil('\r');
          response.trim();
          responseBuffer.concat(response);
        }

        char json[responseBuffer.length() + 1];
        responseBuffer.toCharArray(json, sizeof(json));
        Serial.println(json);
        auto error = deserializeJson(jsonBuffer, json);
        delay(200);
        if (error && responseBuffer.length() == 0) {
          Serial.print("JSON parse error: ");
          Serial.println(error.c_str());
          return;
        }
        const char *interval = jsonBuffer["interval"];
        Serial.println(interval);
        const char *sdi12Address = jsonBuffer["sdi12Address"];
        Serial.println(sdi12Address);

        String intervalString = String(interval);
        String sdi12AddressString = String(sdi12Address);
        if (intervalString == "1" || intervalString == "5" || intervalString == "10" || intervalString == "15" || intervalString == "20" || intervalString == "30" || intervalString == "60") {
          writePreference(4, intervalString);
        }

        writePreference(5, sdi12AddressString);

        Serial.println("closing connection");
        delay(100);
        client.stop();
        return;
      } else {
        Serial.println(".");
        delay(200);
      }
    }
  }
  Serial.println("\r\n-----GET request failed.-----\r\n");
}