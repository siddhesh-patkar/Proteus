#pragma once

#include <Wire.h>
#include <Adafruit_INA219.h>

#ifndef __INA219_H__
#define __INA219_H__

class Ina219 : public Adafruit_INA219
 { public:
      Ina219 (uint8_t a0, uint8_t a1) 
       { // ax = 0 -> FLOAT
         // ax = 1 -> GND
         // ax = 2 -> VCC
         
         // Translates correct logical states.
         (a0>0) ? a0=a0-1 : a0=0; 
         (a1>0) ? a1=a1-1 : a1=0; 
         
         // Create a sub-address from 0 to 3.  
         addr = (a1<<1) | a0;
       }
      // This is required to wrap standard Adafruit method begin and passing correct sub-address.
      void begin() { Adafruit_INA219::begin(addr);}
 
   private:
      uint8_t addr;    
 };
 
#endif