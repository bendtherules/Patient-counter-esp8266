#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <networkCredentials.h>
#include <version.h>
#include <ArduinoOTA.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include "myDisplay.h"

void setupOTALocal() {
  Serial.println("Booting");
  char infoBuffer[LCD_COLUMNS];
  sprintf(infoBuffer, "W:%s", WIFI_SSID);
  showInsideInfo(infoBuffer);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    showInsideInfo("W:Failed");
    delay(5000);
    ESP.restart();
  }
  showInsideInfo("W:Connected");
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
    showInsideInfo("LU:Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    showInsideInfo("LU:Restarting..");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress:%u%%\r", (progress / (total / 100)));
    char infoBuffer[LCD_COLUMNS];
    sprintf(infoBuffer, "LU:P:%u%%", (progress / (total / 100)));
    showInsideInfo(infoBuffer);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]:", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());
}

void handleOTALocal() {
  ArduinoOTA.handle();
}

void handleOTARemote() {
  WiFiClient client;
  
  Serial.println("Updating from remote...");
  showInsideInfo("RU:Start");
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://patient.bendtherules.in/build/esp12e/firmware.bin", VERSION_SHORT);
  switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.print("update:Update failed. Reason - ");
      Serial.println(ESPhttpUpdate.getLastErrorString());
      showInsideInfo("RU:Failed");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("update:Update no Update.");
      showInsideInfo("RU:Skip");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("update:Update ok."); // may not be called since we reboot the ESP
      showInsideInfo("RU:Done");
      break;
  }
}
