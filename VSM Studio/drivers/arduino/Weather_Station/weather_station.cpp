#include "weather_station.h"

WeatherStation::WeatherStation(uint8_t t, uint8_t p, uint8_t h)
 { tout = t; 
   pout = p; 
   hout = h; 
 }

float WeatherStation::readTemperature(void)
 { double sum = 0.0; 
   double T = 0.0;
   int nread = 100;   
   int val = 0;
   for (int i=0; i<nread; i++)
    { val = analogRead(tout);
      T = (((5/1024.0*val)-0.5)* 100);
      sum += T;
    }
   return (sum/nread);
 }
 
float WeatherStation::readPressure(void)
 { double sum = 0.0; 
   double P = 0.0;
   int nread = 100;   
   int val = 0;
   for (int i=0; i<nread; i++)
    { val = analogRead(pout);
      P=(((val*5/1024.0)/5+0.095)/0.009)*10; 
      sum += P;
    }
   return (sum/nread); 
 }
 
float WeatherStation::readHumidity(void)
 { double sum = 0.0; 
   double RH = 0.0;
   int nread = 100;
   int val = 0;
   for (int i=0; i<nread; i++)
    { val = analogRead(hout);              
      RH=(((val*5/1024.0/3.3)-0.1515)/0.00636);
      sum += RH;       
    }
   return (sum/nread);
 }
