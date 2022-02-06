#ifndef TEMP_SENSOR_HPP
#define TEMP_SENSOR_HPP

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "definations.hpp"
#include "NTPTimeDate.hpp"

class TempSensorHelper
{
private:
    Adafruit_BME280 bme;
    bool status;
    float seaLevelPressure = 1013.25f; //hPa
public:
    TempSensorHelper();
    ~TempSensorHelper();
    float getTemp();
    float getPressure();
    float getAltitude();
    float getHumidity();
    String getFormattedData();
    String getFormattedDataForLogging();
    void logData();
    String toHTML();
    String toJson();

};

TempSensorHelper::TempSensorHelper()
{
    status = bme.begin(0x76);  // 0x76 or 0x77
}

TempSensorHelper::~TempSensorHelper()
{
    
}

float TempSensorHelper::getTemp()
{
    return bme.readTemperature();
}

float TempSensorHelper::getPressure()
{
    return bme.readPressure();
}

float TempSensorHelper::getAltitude()
{
    return bme.readAltitude(seaLevelPressure);
}

float TempSensorHelper::getHumidity()
{
    return bme.readHumidity();
}

String TempSensorHelper::getFormattedData()
{
    String data = "";
    data += "Temperature: ";
    data += getTemp();
    data += " C\n";
    data += "Pressure: ";
    data += getPressure();
    data += " hPa\n";
    data += "Altitude: ";
    data += getAltitude();
    data += " m\n";
    data += "Humidity: ";
    data += getHumidity();
    data += " %\n";
    return data;
}

String TempSensorHelper::getFormattedDataForLogging()
{
    String data = "";
    data += getTemp();
    data += ",";
    data += getPressure();
    data += ",";
    data += getAltitude();
    data += ",";
    data += getHumidity();
    data += "\n";
    return data;
}

String TempSensorHelper::toHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Station</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Weather Station</h1>\n";
  ptr +="<p>Temperature: ";
  ptr +=this->getTemp();
  ptr +="&deg;C</p>";
  ptr +="<p>Humidity: ";
  ptr +=this->getHumidity();
  ptr +="%</p>";
  ptr +="<p>Pressure: ";
  ptr +=this->getPressure();
  ptr +="hPa</p>";
  ptr +="<p>Altitude: ";
  ptr +=this->getAltitude();
  ptr +="m</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String TempSensorHelper::toJson(){
  String ptr = "{\n";
  ptr +="\"Temperature\": ";
  ptr +=this->getTemp();
  ptr +="\n";
  ptr +="\"Humidity\": ";
  ptr +=this->getHumidity();
  ptr +="\n";
  ptr +="\"Pressure\": ";
  ptr +=this->getPressure();
  ptr +="\n";
  ptr +="\"Altitude\": ";
  ptr +=this->getAltitude();
  ptr +="\n";
  ptr +="}";
  return ptr;
}

#endif