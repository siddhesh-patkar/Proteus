#pragma once

#include <Wire.h>
#include <Adafruit_Trellis.h>

#ifndef __HT16K33_H__
#define __HT16K33_H__

class HT16K33 : public Adafruit_Trellis
 { public:
      HT16K33 (uint8_t a0, uint8_t a1, uint8_t a2) 
       { // ax = 0 -> open
         // ax = 1 -> close
         // Create address from sub-address = 0 to 7.  
         addr = (a2<<2) | (a1<<1) | a0;
         addr = 0x70 + addr;
       }
      // This is required to wrap standard Adafruit method begin and passing correct address.
      void begin() { Adafruit_Trellis::begin(addr);}
 
   private:
      uint8_t addr;    
 };
 
#endif
