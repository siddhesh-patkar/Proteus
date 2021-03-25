#pragma once

#include <arduino.h>

class DataLogger
 { public:
      DataLogger (uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t); 

      void begin(void) { };
      void setRedLed  (bool s);
      void setGreenLed (bool s);
      void toggleRedLed(void);
      void toggleGreenLed(void);
      void writeDigital(uint8_t p, bool s);   
      bool readDigital(uint8_t p);            
      uint16_t readRaw (uint8_t);
      float readVoltage (uint8_t, uint8_t nav=1, float ref=5.0);
   
   private:
      uint8_t pinRedLed;
      uint8_t pinGreenLed;
      uint8_t pinDig0, pinDig1, pinDig2, pinDig3;
      uint8_t pinAn0,  pinAn1,  pinAn2,  pinAn3;
 };
