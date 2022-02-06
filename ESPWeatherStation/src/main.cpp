#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include "SDCardLogger.hpp"
#include "definations.hpp"
#include "NTPTimeDate.hpp"
#include "TempSensorHelper.hpp"

SDCardLogger Log(CS_PIN);
NTPTimeDate timeClient;
TempSensorHelper TempSensor(&Log, &timeClient);
ESP8266WebServer server(80);

unsigned long previousMillis = 0;
unsigned long interval = 10000;

void handeleMain(){
  server.send(200, "text/plain", TempSensor.toHTML());
}
void handeleRest(){
  server.send(200, "text/plain", TempSensor.toJson());
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFiManager wifiManager;
  wifiManager.autoConnect("Weather Station");

  while (WiFi.status() != WL_CONNECTED) { 
    digitalWrite(LED_BUILTIN, LOW);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  server.on("/", handeleMain);
  server.on("/api", handeleRest);
  Log.enableSDLogging();
}

void loop() {
 unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    TempSensor.logData();
  }
  server.handleClient();
  timeClient.NTPUpdate();
}