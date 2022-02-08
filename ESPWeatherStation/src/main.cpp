#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include "definations.hpp"
#include <SD.h>
#include <SPI.h>
#include <FS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#define NTP_OFFSET 3600

ESP8266WebServer server(80);
Adafruit_BME280 bme;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"tr.pool.ntp.org", NTP_OFFSET * 3 );
LiquidCrystal_I2C lcd(0x27,16,2);  

#define SEALEVELPRESSURE_HPA (1013.25)
unsigned long previousMillis = 0;
unsigned long previousMillisLCD = 0;

unsigned long interval = 600000;
unsigned long intervalLCD = 3000;


String Now()
{
  char buf[80];
  struct tm  ts;
  time_t epochTime = timeClient.getEpochTime();
  ts = *localtime(&epochTime);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
  return String(buf);
  }


void writeHeader()
{
    if (!SD.exists("log.csv"))
    {
        File logFile = SD.open("log.csv", FILE_WRITE);
        logFile.print("TimeStamp,Temperature,Pressure,Altitude,Humidity\n");
        Serial.println("SD Printing Header to log file...");
        logFile.close();
    }
}

void SDWriteLog(String TimeStamp, String Temperature, String Pressure, String Altitude, String Humidity)
{
    File logFile = SD.open("log.csv", FILE_WRITE);
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


String HTML(){
    String rText = "<!DOCTYPE html> <html> <head> <script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\"></script></script><script src=\"https://code.jquery.com/jquery-3.6.0.min.js\" integrity=\"sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4=\" crossorigin=\"anonymous\"></script><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"> <title>Weather Station</title> <style>html{font-family:Helvetica;display:inline-block;margin:0 auto;text-align:center}body{margin-top:50px}h1{color:#444;margin:50px auto 30px}p{font-size:24px;color:#444;margin-bottom:10px}</style></head><body> <div id=\"webpage\"> <h1>Weather Station</h1> <p>Temperature: <span id=\"temp\">0</span> &deg;C</p> <p>Humidity: <span id=\"hum\">0</span> %</p> <p>Pressure: <span id=\"pres\">0</span> hPa</p> <p>Altitude: <span id=\"alt\">0</span> m</p> </div> <canvas id=\"mycanvas\"></canvas> <canvas id=\"Pressure\"></canvas> <script>// create initial empty chart\nvar ctx_live = document.getElementById(\"mycanvas\");\n        var myChart = new Chart(ctx_live, {\ntype: \'line\',\ndata: {\nlabels: [],\ndatasets: [{\ndata: [],\nborderWidth: 1,\nborderColor: \'green\',\nlabel: \'Temperature\',\n},\n{\ndata: [],\nborderWidth: 1,\nborderColor: \'blue\',\nlabel: \'Humidity\',\n},\n]\n},\noptions: {\nresponsive: true,\n                title: {\n                    display: true,\n                    text: \"Temperature & Humidity Chart\",\n                },\n                legend: {\n                    display: true,\n                },\n                scales: {\n                    yAxes: [{\n                        display: true,\n                        ticks: {\n                            beginAtZero: true,\n                        }\n                    }],\n                    xAxes: [{\n                        display: true,\n                        ticks: {\n                            autoSkip: true,\n                            maxTicksLimit: 20,\n                            beginAtZero: true,\n                        }\n                    }]\n\n                }\n            }\n        });\n\n        // create initial empty chart\n        var ctx_live2 = document.getElementById(\"Pressure\");\n        var pressureChart = new Chart(ctx_live2, {\n            type: \'line\',\n            data: {\n                labels: [],\n                datasets: [{\n                        data: [],\n                        borderWidth: 1,\n                        borderColor: \'green\',\n                        label: \'Pressure\',\n                    },\n                    {\n                        data: [],\n                        borderWidth: 1,\n                        borderColor: \'yellow\',\n                        label: \'Altitude\',\n                    }\n                ]\n            },\n            options: {\n                responsive: true,\n                title: {\n                    display: true,\n                    text: \"Pressure Chart\",\n                },\n                legend: {\n                    display: true,\n                },\n                scales: {\n                    yAxes: [{\n                        display: true,\n                        ticks: {\n                            beginAtZero: true,\n                        }\n                    }],\n                    xAxes: [{\n                        display: true,\n                        ticks: {\n                            autoSkip: true,\n                            maxTicksLimit: 20,\n                            beginAtZero: true,\n                        }\n                    }]\n\n                }\n            }\n        });\n\n        function csvToArray(str, delimiter = \",\") {\n            const headers = str.slice(0, str.indexOf(\"\\n\")).split(delimiter);\n            const rows = str.slice(str.indexOf(\"\\n\") + 1).split(\"\\n\");\n            const arr = rows.map(function (row) {\n                const values = row.split(delimiter);\n                const el = headers.reduce(function (object, header, index) {\n                    object[header] = values[index];\n                    return object;\n                }, {});\n                return el;\n            });\n            return arr;\n        }\n\n        var getData = function () {\n            var data;\n            $.ajax({\n                url: window.location.href + \"log.csv\",\n                async: false,\n                success: function (csvd) {\n                    data = csvToArray(csvd);\n                }\n            });\n            myChart.data.labels = [];\n            myChart.data.datasets[0].data = [];\n            myChart.data.datasets[1].data = [];\n     pressureChart.data.labels = []; \n       pressureChart.data.datasets[0].data = [];\n            pressureChart.data.datasets[1].data = [];\n            data.forEach(element => {\n                myChart.data.labels.push(element[\"TimeStamp\"]);\n                myChart.data.datasets[0].data.push(element[\"Temperature\"]);\n                myChart.data.datasets[1].data.push(element[\"Humidity\"]);\n                pressureChart.data.labels.push(element[\"TimeStamp\"]);\n                pressureChart.data.datasets[0].data.push(element[\"Pressure\"]);\n                pressureChart.data.datasets[1].data.push(element[\"Altitude\"]);\n            })\n            myChart.update();\n            pressureChart.update();\n        };\n\n        var reLoadData = function () {\n            $.getJSON( window.location.href + \"api\", function (data) {\n            $(\"#temp\").text(data[\"Temperature\"]);\n            $(\"#hum\").text(data[\"Humidity\"]);\n            $(\"#pres\").text(data[\"Pressure\"]);\n            $(\"#alt\").text(data[\"Altitude\"]);\n            myChart.update();\n            pressureChart.update();\n        });\n        }\n        setInterval(reLoadData, 3000);\n        setInterval(getData, 30000);\n        \n        $(document).ready(function(){getData(); reLoadData();});</script></body></html>";
        return rText;
  }

String Json(){
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
// String csvFileToJson(){
//   String last50Line = "";

//   File logFile = SD.open("log.csv", FILE_READ);
//   while (logFile.available()) {
//     yield();
//     last50Line += logFile.readStringUntil('\n');
//   }

// }

bool loadFromSdCard(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.html";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  else if(path.endsWith(".csv")) dataType = "text/text";
  else if(path.endsWith(".js"))  dataType = "text/javascript";

  File dataFile = SD.open(path.c_str());
  if(dataFile.isDirectory()){
    path += "/index.html";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile)
    return false;

  if (server.hasArg("download")) dataType = "application/octet-stream";

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

void putSerial(){
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

void handeleMain(){
  server.send(200, "text/html", HTML());
}
void handeleRest(){
  server.send(200, "text/plain", Json());
}
void returnOK() {
  server.send(200, "text/plain", "");
}

void returnFail(String msg) {
  server.send(500, "text/plain", msg + "\r\n");
}

void printToLCD(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(bme.readTemperature());
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Press: ");
  lcd.print(bme.readPressure() / 100.0F);
  lcd.print("hPa");
}
void handleNotFound(){
  if(loadFromSdCard(server.uri())) return;
  String message = "SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}

void printDirectory() {
  if(!server.hasArg("dir")) return returnFail("BAD ARGS");
  String path = server.arg("dir");
  if(path != "/" && !SD.exists((char *)path.c_str())) return returnFail("BAD PATH");
  File dir = SD.open((char *)path.c_str());
  path = String();
  if(!dir.isDirectory()){
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/json", "");
  WiFiClient client = server.client();

  server.sendContent("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry)
    break;

    String output;
    if (cnt > 0)
      output = ',';

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.name();
    output += "\"";
    output += "}";
    server.sendContent(output);
    entry.close();
 }
 server.sendContent("]");
 dir.close();
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  SD.begin(CS_PIN);
  lcd.init();
  lcd.backlight();
  bool status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  WiFi.hostname("Weather Station");
  WiFi.config(IPAddress(192, 168, 0, 100), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0)); 
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  //WiFi.begin("ArasNet", "Ar102797");
  Serial.println("Wifi not connected");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  if (MDNS.begin("weather")) {  //Start mDNS with name esp8266
      Serial.println("MDNS started");
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

  Serial.println("Wifi Connected");
  server.on("/", handeleMain);
  Serial.println("main handler init...");
  server.on("/api", handeleRest);
  Serial.println("rest handler init...");
  server.on("/list", HTTP_GET, printDirectory);
  server.onNotFound(handleNotFound);
  server.begin();
  timeClient.begin();

  writeHeader();
  printToLCD();
}

void loop() {
 unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    SDWriteLog(Now(),String(bme.readTemperature()),String((bme.readPressure() / 100.0F)),String(bme.readAltitude(SEALEVELPRESSURE_HPA)),String(bme.readHumidity()));
  }
  unsigned long currentMillisLCD = millis();
  if (currentMillisLCD - previousMillisLCD >= intervalLCD) {
    previousMillisLCD = currentMillisLCD;
    printToLCD();
  }
  server.handleClient();
  timeClient.update();
  ArduinoOTA.handle();
  MDNS.update();
}