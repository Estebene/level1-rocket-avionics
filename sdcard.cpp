#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"
#include "sdcard.h"
#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;
SdFat SD;
File myFile;

const uint8_t chipSelect = SS;

static float getCurrentHeight(float pressure, float temp, int groundPressure) {
  float pressureCalc = pow((groundPressure * 100)/(pressure), 0.190223) - 1;
  float tempCalc = temp + 273.15;
  return (pressureCalc * tempCalc)/0.0065;
}

void getHTML(char* html, char* filename) {
  uint32_t index = 0;
   myFile = SD.open(filename);
  if (myFile) {
    while (myFile.available()) {
      char c = myFile.read();
      *(html + index) = c;
      index++;
    }
    myFile.close();
  } else {
    Serial.println("error opening file");
  }
}

String bmpInit(void) {
  if (!bmp.begin()) {
    return "BMP280 Not Connected";
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  return "";
}

String sdInit(void) {
  if (!SD.begin(chipSelect, SD_SCK_MHZ(50))) {
    return "SD Card Issue, Fuck";
  }
  return "";
}

String sdLogs(char* filename, long logTime, int groundPressure) {
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    BmpResult_t data = getBmpData(groundPressure);
    myFile.print(logTime);
    myFile.print(",");
    myFile.print(data.temp);
    myFile.print(",");
    myFile.print(data.pressure);
    myFile.print(",");
    myFile.print(data.altitude);
    myFile.print(",");
    myFile.println();
    myFile.close();
  } else {
    return "file opening error";
  }
  return "";
}

String sdHeaders(char* filename) {
   myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    myFile.println("logTime, temperature, pressure, altitude");
    myFile.close();
  } else {
    return "file opening error";
  }
  return "";
}

BmpResult_t getBmpData(int groundPressure) {
  BmpResult_t data;
  Serial.println(bmp.readTemperature());
  Serial.println(float(bmp.readTemperature()));
  data.temp = float(bmp.readTemperature());
  Serial.println(data.temp);
  data.pressure = float(bmp.readPressure());
  data.altitude = float(getCurrentHeight(data.pressure, data.temp, groundPressure));
  return data;
}
