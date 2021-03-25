//  HYT271HumiTempSensor.cpp - Driver for HYT271 precision humidity and temperature sensors.

#include "Arduino.h"
#include "HYT271.h"
#include "Wire.h"

// Address is provided at constructor level. 
// Cames from DVCADDR property and can be any number from 0 to 0x7f.
HYT271HumiTempSensor::HYT271HumiTempSensor(uint8_t addr)
 { // Get final address.
   _addr = addr>>1;
 }

void HYT271HumiTempSensor::begin ()
 { //set arduino as master:
   Wire.begin();
   humidity = 0;
   temperature = 0;
 } 

void HYT271HumiTempSensor::readSensor(void)
 { uint8_t tempHbyte, tempLbyte; 
   uint8_t humiHbyte, humiLbyte; 
   
   // Dummy byte:
   Wire.write(0);
   Wire.beginTransmission(_addr);
   Wire.endTransmission();
   delay(50);
   
   //DataFetch(4 byte):
   Wire.requestFrom(_addr, 4);
   // wait for 4 bytes available:   
   if(Wire.available()>3)
    { // Receive the data bytes:
      humiHbyte = Wire.read();
      humiLbyte = Wire.read();
      tempHbyte = Wire.read();
      tempLbyte = Wire.read();   
      Wire.beginTransmission(_addr);
      Wire.endTransmission();
      
      // Mask the status bits
      humiHbyte = (humiHbyte & B00111111);
      tempLbyte = (tempLbyte & B11111100);
      
      // Put together the highbytes and lowbytes 
      // For this, the highbyte gets moved 8 bits higher 
      humiValue  = (humiHbyte*256 + humiLbyte);
      //the temperature highbyte gets moved 6 bits and the lowbyte 2 bits lower. 
      tempValue = ((tempHbyte)*64 + tempLbyte/4);
      
      // Final Humidity/Temperature values:
      humidity = (double(humiValue))/163.83;
      temperature = ((double(tempValue))/16383)*165-40;
      initialized = true;
    }
 }

float HYT271HumiTempSensor::getTemperature(bool F)
 { // Return temperature previously sampled by readSensor()
   if (F)
      // Temperature Scale in Farenheit. 
      return temperature * 9 / 5 + 32;
   else
      // Temperature Scale in Celsius. 
      return temperature;
 }

float HYT271HumiTempSensor::getHumidity()
 { // Return relative humidity previously sampled by readSensor().
   return humidity;
 } 

float HYT271HumiTempSensor::readTemperature(bool F)
 { temperature = 0;
   HYT271HumiTempSensor::readSensor();
   // Return temperature:
   if (F)
      // Temperature Scale in Farenheit. 
      return temperature * 9 / 5 + 32;
   else
      // Temperature Scale in Celsius. 
      return temperature;
 } 


float HYT271HumiTempSensor::readHumidity()
 { humidity = 0;
   HYT271HumiTempSensor::readSensor();
   return humidity;
 } 

