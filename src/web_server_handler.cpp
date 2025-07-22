#include "web_server_handler.h"
#include "storage_handler.h"
#include "sensor_handler.h"
#include "index_html.h"
#include "version.h"
#include <WiFi.h>
#include <FastLED.h>

extern CRGB led[];
extern int bootMode;
extern const char *MAINTENANCE_MODE;
extern const char *NORMAL_MODE;

void handleNotFound() {
  Server.send(404, "text/plain", "Not found");
}

void handleRoot() {
  Serial.println("\r\n------------------------------------------------");
  Serial.println("Processing the request.....");

  if (Server.method() == HTTP_POST) {
    String ssid_edit;
    String pass_edit;
    String address_edit;
    String present_address;
    String following_address;
    String host_edit;
    String reboot;
    String maintenance_mode;
    String normal_mode;

    ssid_edit = Server.arg("ssid");
    pass_edit = Server.arg("pass");
    address_edit = Server.arg("address");
    present_address = Server.arg("present_address");
    following_address = Server.arg("following_address");
    host_edit = Server.arg("host");
    reboot = Server.arg("reboot");
    maintenance_mode = Server.arg("maintenance");
    normal_mode = Server.arg("normal");

    if (reboot != "") {
      led[0] = 0x000000;
      FastLED.show();
      delay(100);
      ESP.restart();
    }
    if (ssid_edit != "" || pass_edit != "") {
      writePreference(1, ssid_edit);
      writePreference(2, pass_edit);
      Serial.println("Changing access point setting...");
      Serial.print("SSID:");
      Serial.println(ssid_edit);
      Serial.print("PASS:");
      Serial.println(pass_edit);
    } else if (address_edit != "" && present_address && following_address) {
      Serial.println("Change SDI-12 sensor address...");
      Serial.print("Present address:");
      Serial.print(present_address);
      Serial.print("Following address:");
      Serial.print(following_address);
      bool result = addressChange(present_address, following_address);
    } else if (maintenance_mode != "" | normal_mode != "") {
      if (maintenance_mode != "") {
        writePreference(0, String(MAINTENANCE_MODE));
        writePreference(3, host_edit);
        Serial.println("Changing HOST...");
        Serial.print("New HOST: ");
        Serial.println(host_edit);
      } else {
        Serial.println("Changing BootMode...");
        writePreference(0, String(NORMAL_MODE));
      }
    }
  }
  String macAddress = WiFi.macAddress();
  const char *macAddress_pointer = macAddress.c_str();
  String ssidString = readPreference(1);
  const char *ssid = ssidString.c_str();
  const char *version = VERSION.c_str();
  String bootModeString = readPreference(0);
  const char *bootMode = bootModeString.c_str();

  String htmlString = generateHtml(macAddress_pointer, ssid, version, bootMode);

  Serial.println("------------------------------------------------\r\n");

  Server.send(200, "text/html", htmlString);
}