// Wrapper for the SPI port

#pragma once

#include <core.h>
#include <SPI.h>


class SpiList 
 { public:
      SpiList &begin(uint8_t w); 
      SpiList &arg(int16_t i); 
      SpiList &arg(const char *s); 
      SpiList &arg(const String &s); 
      void end() {};
   private:
      bool wide;
 };
      
class SpiPort
 { public:      
      void begin() { SPI.begin(); ss=-1; }
      void setup (long clock, uint8_t order, uint8_t mode) { settings=SPISettings(clock, order, mode);  }
      void beginTransaction(uint8_t pin);
      void endTransaction();
      SpiList &send() { return list.begin(false); }
      SpiList &send16() { return list.begin(true); }
      uint8_t transfer(uint8_t data) { return SPI.transfer(data); }
      uint16_t transfer16(uint16_t data) { return SPI.transfer(data); }
   private:
      SPISettings settings;
      uint8_t ss;
      SpiList list;
            
 };       

extern SpiPort Spi;

