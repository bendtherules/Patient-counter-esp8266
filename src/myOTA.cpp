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
  {
    char infoBuffer[LCD_COLUMNS];
    sprintf(infoBuffer, "W:%s", WIFI_SSID);
    showInsideInfo(infoBuffer);
  }
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
    Serial.printf("LU: Progress:%u%%\r", (progress / (total / 100)));
    {
      char infoBuffer[LCD_COLUMNS];
      sprintf(infoBuffer, "LU:P:%u%%", (progress / (total / 100)));
      showInsideInfo(infoBuffer);
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]:", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    showInsideInfo("LU:Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());
  showInsideInfo("Ready");
}

void printIPInfo() {
  String localIPStr = WiFi.localIP().toString();
  char localIP[LCD_COLUMNS];
  localIPStr.toCharArray(localIP, LCD_COLUMNS);
  char infoBuffer[LCD_COLUMNS];
  sprintf(infoBuffer, "W:%s", localIP);
  showInsideInfo(infoBuffer);
}

void handleOTALocal() {
  ArduinoOTA.handle();
}

void handleOTARemote() {
  WiFiClient client;
  
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  ESPhttpUpdate.onStart([]() {
    Serial.println("RU: Started");
    showInsideInfo("RU:Started");
  });
  ESPhttpUpdate.onEnd([]() {
    Serial.println("RU: Finished. Restarting..");
    showInsideInfo("RU:Restarting..");
  });
  ESPhttpUpdate.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("RU: Progress:%u%%\r", (progress / (total / 100)));
    {
      char infoBuffer[LCD_COLUMNS];
      sprintf(infoBuffer, "RU:P:%u%%", (progress / (total / 100)));
      showInsideInfo(infoBuffer);
    }
  });
  ESPhttpUpdate.onError([](int err) {
    Serial.printf("RU: Failed with code %d\n", err);
    Serial.println(ESPhttpUpdate.getLastErrorString());
    showInsideInfo("RU:Failed");
  });

  t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://patient.bendtherules.in/build/esp12e/firmware.bin");
}
