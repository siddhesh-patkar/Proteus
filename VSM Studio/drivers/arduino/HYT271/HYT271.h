//  HYT271HumiTempSensor.h - header for HYT271 precision humidity and temperature sensors.

#ifndef __HYT271HumiTempSensor_H__
#define __HYT271HumiTempSensor_H__

#include "Arduino.h"
#include "Wire.h"

class HYT271HumiTempSensor 
 { public:
      HYT271HumiTempSensor(uint8_t addr);
      void begin ();
      void readSensor();
      float getTemperature(bool F);
      float getHumidity();
      float readTemperature(bool F);
      float readHumidity();
      
   private:
      uint16_t tempValue;
      uint16_t humiValue;
      double temperature;
      double humidity;
      int _addr;
      bool initialized;
 };

#endif

