#pragma once

#include <Wire.h>
#include <i2c_adc.h>

class GroveI2CAdc : public I2C_ADC
 { public:
      GroveI2CAdc (uint8_t adr0, uint8_t adr1) { a0 = adr0, a1 = adr1; };
      int readAnalog() { return readValue(1); }
      int readAverageAnalog(int navr)  { return readValue(navr); }
      float readVoltage() { return (readValue(10)<<1)*3.0/4096.0; } // Takes into the accout the 1:2 input partition ratio. 
 	
 	private:
 	   int readValue(int num_average) { if (num_average>100) num_average=100; an=0; for (idx=0; idx<num_average; idx++) an = an + read_i2c_adc() ; return an/num_average; }  
 	
 	private:
 	   long an;
 	   uint8_t idx;
 };     