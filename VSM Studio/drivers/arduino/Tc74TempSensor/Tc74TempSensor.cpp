//  TC74TempSensor.cpp - Library for TC74 temperature sensors.

#include "Arduino.h"
#include "TC74TempSensor.h"
#include "Wire.h"

// Address is provided at constructor level.
TC74TempSensor::TC74TempSensor(uint8_t addr)
 { // Get final address.
   _addr = addr>>1;
 }

void TC74TempSensor::begin (uint8_t addr)
 { // Get final address.
   _addr = addr>>1;
 } 

signed int TC74TempSensor::read(void)
 { int _temp = 255;
   // Get reading from the temperature sensor.
   Wire.beginTransmission(_addr);
   Wire.write((uint8_t) 0); 
   Wire.endTransmission();
    
   // Request one byte from sensor.
   Wire.requestFrom((int) _addr, (int) 1);
    
   _temp = Wire.read();
        
   if (_temp > 127)
      _temp = -(255 - _temp + 1);
       
   return _temp;
 }


