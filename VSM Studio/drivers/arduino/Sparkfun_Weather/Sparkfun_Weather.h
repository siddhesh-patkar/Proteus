#pragma once

#include <arduino.h>

#include "MPL3115A2.h"
#include "Si7021.h"

class WeatherStation
 { public:
      WeatherStation (uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
      void begin();
      void poll();
      
      void reset();
      float getTemperature(void);
      float getPressure(void);
      float getHumidity(void);
      float getDaylight();
      float getWindSpeed();
      float getWindDirection();
      float getGustSpeed();
      float getGustDirection();
      float getRainHour();
      float getRainDay();
   
   private:  
      static void rainIRQ();
      static void wspeedIRQ();
      static volatile unsigned long rainlastIRQ, rainticks;
      static volatile unsigned long windlastIRQ, windticks;

      float rawWindSpeed();
      float rawWindDirection();
      
   private:
      uint8_t wdout, wsout, lsref, lsout, rgout;
      float rainTotal;
      MPL3115A2 pressure;
      Si7021 humidity;
      
      unsigned long lastpoll, nextpoll;
      byte second; //When it hits 60, increase the current minute
      byte minute; //Keeps track of where we are in various arrays of data
      byte hour;
      
      byte windspeedavg[60]; //60 bytes to keep track of 1 minute average
      int winddiravg[60]; //60 ints to keep track of 1 minute average
      byte windgust_10m[10]; //10 values to keep track of 10 minute max
      int windgustdir_10m[10]; //10 ints to keep track of 10 minute max
      float raintenminutes;
      int rainhour[6]; //6  numbers to keep track of 6x10 minutes of rain
      int rainday[24]; //24 floating numbers to keep track of 24 hours of rain.

 };
