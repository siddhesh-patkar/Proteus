//  TC74TempSensor.h - Library for TC74 temperature sensors.

#ifndef TC74TempSensor_h
#define TC74TempSensor_h

#include "Arduino.h"
#include "Wire.h"

class TC74TempSensor 
 { public:
      TC74TempSensor(uint8_t addr);
      void begin (uint8_t addr);
      signed int read();
   
   private:
      uint8_t _addr;
 };

#endif

