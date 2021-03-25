#pragma once

class GroveDustSensor 
 { public:
      GroveDustSensor();
      void begin(int pin);
      void poll();
      void setSamplePeriod(int p);
      void setNumSamples(int ns);
      long readDustNow () { return concentration; }
      long readDustAvg ();
   private:
      long sampleperiod;
      long nextsample;
      int  numsamples;
      long concentration;
      long *readings;
      int  reading;
            
      
 };
