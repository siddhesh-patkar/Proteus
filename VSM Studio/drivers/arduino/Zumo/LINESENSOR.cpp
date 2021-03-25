#include <arduino.h>
#include "Zumo.h"

void LINESENSOR::begin()
 { unsigned char pins[6] = { lh0, lh1, lh2, lh3, lh4, lh5 };
   init(pins, 6, 1000);

   calibratedMinimumOn = (unsigned int*)malloc(sizeof(unsigned int)*6);
   calibratedMinimumOff = (unsigned int*)malloc(sizeof(unsigned int)*6);
   calibratedMaximumOn = (unsigned int*)malloc(sizeof(unsigned int)*6);
   calibratedMaximumOff = (unsigned int*)malloc(sizeof(unsigned int)*6);

   for (int i=0; i<6; i++)
    { calibratedMinimumOn[i] = 100;
      calibratedMinimumOff[i] = 100;
      calibratedMaximumOn[i] = 1000;
      calibratedMaximumOff[i] = 1000;
    }
 }

bool LINESENSOR::operator() (int min, int max)
 { int pos = readLinePos();
   return (pos >= min) && (pos <= max);
 }

int LINESENSOR::readLinePos()
 { unsigned int sensor_values[6] = { 0 };
   return readLine(sensor_values);
 }
 
 int LINESENSOR::readRawValue(int index)
 { unsigned int sensor_values[6] = { 0 };
   readPrivate(sensor_values);
   return sensor_values[index];
 }