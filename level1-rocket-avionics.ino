/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-nodemcu-access-point-ap-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include "sdcard.h"

const char* ssid     = "Voyager_3_WIFI";
const char* password = "tothemoon";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Updates DHT readings every 10 seconds
const long interval = 10000;


void setup(){
  Serial.begin(115200);
 
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  sdInit();
  bmpInit();

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){   
    char buffer[30000];
    getHTML(buffer, "test.html");
    request->send(200, "text/html", buffer);
    
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    BmpResult_t data = getBmpData();
    String dataStr = "{\"data\": [";
    Serial.println(data.temp);
    dataStr += String(data.temp);
    dataStr += ", ";
    dataStr += String(data.pressure);
    dataStr += ", ";
    dataStr += String(data.altitude);
    dataStr += "]}";
    request->send(200, "text/plain", dataStr);
  });
  

  // Start server
  server.begin();
}
 
void loop(){  
 
}
