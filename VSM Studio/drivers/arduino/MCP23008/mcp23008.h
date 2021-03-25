#pragma once

#include <Wire.h>
#include <Adafruit_MCP23008.h>

#ifndef __MCP23008_H__
#define __MCP23008_H__

class MCP23008 : public Adafruit_MCP23008
 { public:
      MCP23008 (uint8_t a0, uint8_t a1, uint8_t a2) 
       { // ax = 0 -> floating
         // ax = 1 -> GND
         // ax = 2 -> VCC
         
         // Translates correct logical states.
         (a0>0) ? a0=a0-1 : a0=0; 
         (a1>0) ? a1=a1-1 : a1=0; 
         (a2>0) ? a2=a2-1 : a2=0; 
         
         // Create a sub-address from 0 to 7.  
         addr = (a2<<2) | (a1<<1) | a0;
       }
      // This is required to wrap standard Adafruit method begin and passing correct sub-address.
      void begin() { Adafruit_MCP23008::begin(addr);}
 
   private:
      uint8_t addr;    
 };
 
#endif