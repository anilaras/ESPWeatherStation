#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include "definations.hpp"
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define NTP_OFFSET 3600

ESP8266WebServer server(80);
Adafruit_BME280 bme;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"tr.pool.ntp.org", NTP_OFFSET * 3 );

#define SEALEVELPRESSURE_HPA (1013.25)
unsigned long previousMillis = 0;
unsigned long interval = 60000;

String Now()
{
  char buf[80];
  struct tm  ts;
  time_t epochTime = timeClient.getEpochTime();
  ts = *localtime(&epochTime);
  strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &ts);

  //   1990-06-20 08:03:00
  return String(buf);
  }


void writeHeader()
{
    if (!SD.exists("log.cvs"))
    {
        File logFile = SD.open("log.cvs", FILE_WRITE);
        logFile.print("TimeStamp,Temperature,Pressure,Altitude,Humidity\n");
        Serial.println("SD Printing Header to log file...");
    }
}

void SDWriteLog(String TimeStamp, String Temperature, String Pressure, String Altitude, String Humidity)
{
    File logFile = SD.open("log.cvs", FILE_WRITE);
    logFile.print(TimeStamp);
    logFile.print(",");
    logFile.print(Temperature);
    logFile.print(",");
    logFile.print(Pressure);
    logFile.print(",");
    logFile.print(Altitude);
    logFile.print(",");
    logFile.print(Humidity);
    logFile.print("\n");
    logFile.close();
}



String toHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Weather Station</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Weather Station</h1>\n";
  ptr +="<p>Temperature: ";
  ptr += bme.readTemperature();
  ptr +="&deg;C</p>";
  ptr +="<p>Humidity: ";
  ptr +=bme.readHumidity();
  ptr +="%</p>";
  ptr +="<p>Pressure: ";
  ptr +=bme.readPressure() / 100.0F;
  ptr +="hPa</p>";
  ptr +="<p>Altitude: ";
  ptr +=bme.readAltitude(SEALEVELPRESSURE_HPA);
  ptr +="m</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String toJson(){
  String ptr = "{";
  ptr +="\"Temperature\": ";
  ptr +=bme.readTemperature();
  ptr +=",";
  ptr +="\"Humidity\": ";
  ptr +=bme.readHumidity();
  ptr +=",";
  ptr +="\"Pressure\": ";
  ptr +=bme.readPressure() / 100.0F;
  ptr +=",";
  ptr +="\"Altitude\": ";
  ptr +=bme.readAltitude(SEALEVELPRESSURE_HPA);
  ptr +="}";
  return ptr;
}

void handeleMain(){
  server.send(200, "text/html", toHTML());
}
void handeleRest(){
  server.send(200, "text/plain", toJson());
}



void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  SD.begin(CS_PIN);
  
  bool status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  //WiFi.begin("ArasNet", "Ar102797");
  Serial.println("Wifi not connected");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi Connected");
  server.on("/", handeleMain);
  Serial.println("main handler init...");
  server.on("/api", handeleRest);
  Serial.println("rest handler init...");
  server.begin();
  timeClient.begin();
}

void loop() {
 unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    SDWriteLog(Now(),String(bme.readTemperature()),String((bme.readPressure() / 100.0F)),String(bme.readAltitude(SEALEVELPRESSURE_HPA)),String(bme.readHumidity()));
    Serial.println(Now());
    Serial.print("Temperature: ");  
    Serial.print(bme.readTemperature());
    Serial.println(" C");
    Serial.print("Pressure: ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
    Serial.print("Humidity: ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    Serial.print("Altitude: ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");
    }
  server.handleClient();
  timeClient.update();
}