#pragma once

#include <Wire.h>
#include <Adafruit_BMP085.h>

#ifndef __BMP180_H__
#define __BMP180_H__

class BMP180 : public Adafruit_BMP085
 { public:
      bool operator()(float trigger)                        { return readPressure() >= trigger; }
      void  setMode(uint8_t mode)                           { Adafruit_BMP085::setMode(mode); }   
      // These functions are wrapped here as we want the Pressure value in 
      // hPa not in Pa, for consistence with BMP180 active component setting.  
      float readPressure(void)                              { return Adafruit_BMP085::readPressure()/100.0; }
      float readAltitude(float sealevelPressure=1013.25)    { return Adafruit_BMP085::readAltitude(sealevelPressure*100.0); }
      float readSealevelPressure(float altitude_meters=0.0) { return (float) Adafruit_BMP085::readSealevelPressure(altitude_meters)/100.0; }
 };
 
#endif
