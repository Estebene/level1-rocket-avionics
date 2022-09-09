#ifndef SDCARD_H
#define SDCARD_H

#include "Arduino.h"

typedef struct {
  float temp;
  float pressure;
  float altitude;
} BmpResult_t;

void getHTML(char* html, char* filename);
String bmpInit(void);
String sdInit(void);
String sdLog(char* filename, long logTime);
String sdHeaders(char* filename);
BmpResult_t getBmpData();


#endif
