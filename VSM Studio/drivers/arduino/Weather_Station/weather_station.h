#pragma once

#include <arduino.h>

class WeatherStation
 { public:
      WeatherStation (uint8_t, uint8_t, uint8_t);

      float readTemperature(void);
      float readPressure(void);
      float readHumidity(void);
     
   private:
      uint8_t tout, pout, hout;
 };
