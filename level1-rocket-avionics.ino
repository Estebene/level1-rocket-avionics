#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>
#include "sdcard.h"

const char* ssid     = "Voyager_3_WIFI";
const char* password = "tothemoon";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int mode = 0;
int groundPressure = 0;

char* filename = "flight_log.csv";

void writeIntIntoEEPROM(int address, int number) {
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
  EEPROM.commit();
}

int readIntFromEEPROM(int address) {
  return (EEPROM.read(address) << 8) + EEPROM.read(address+1);
}


void setup(){
  Serial.begin(115200);
  EEPROM.begin(512);
 
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  int pressure = readIntFromEEPROM(0);
  Serial.println(pressure);

  if (pressure > 500 && pressure < 2000) {
    groundPressure = pressure;
  } else {
    groundPressure = 1029;
    writeIntIntoEEPROM(0, groundPressure);
  }

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
    BmpResult_t data = getBmpData(groundPressure);
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

  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasHeader("seaLevelPressure")){
      AsyncWebHeader* h = request->getHeader("seaLevelPressure");
      String header =  h->value().c_str();
      int pressure = header.toInt();
      Serial.println(pressure);
      if (pressure > 500 && pressure < 2000) {
        groundPressure = pressure;
        writeIntIntoEEPROM(0, pressure);
        request->send(200);
      } else {
        request->send(500);
      }
    }
  });

  server.on("/mode", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasHeader("mode")) {
      AsyncWebHeader* h = request->getHeader("mode");
      String header = h->value().c_str();
      if (header == "record") {
        mode = 1;
      } else if (header == "standby") {
        mode = 0;
      }
      request->send(200);
    }
  });
  
  // Start server
  server.begin();
}
 
void loop(){  
  if (mode == 1) {
    sdLogs(filename, millis(), groundPressure);
  }
}
