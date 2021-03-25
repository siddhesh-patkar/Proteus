#include "datalogger.h"

DataLogger::DataLogger (uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5, uint8_t p6, uint8_t p7, uint8_t p8, uint8_t p9) 
 { pinRedLed = p0;   pinMode(pinRedLed,   OUTPUT); 
   pinGreenLed = p1; pinMode(pinGreenLed, OUTPUT); 
   pinDig0 = p2; pinMode(pinDig0, INPUT); 
   pinDig1 = p3; pinMode(pinDig1, INPUT); 
   pinDig2 = p4; pinMode(pinDig2, INPUT); 
   pinDig3 = p5; pinMode(pinDig3, INPUT); 
 }

void DataLogger::setRedLed(bool s)
 { digitalWrite(pinRedLed, s ? HIGH : LOW); 
 }

void DataLogger::setGreenLed(bool s)
 { digitalWrite(pinGreenLed, s ? HIGH : LOW); 
 }

void DataLogger::toggleRedLed(void)
 { digitalWrite(pinRedLed, digitalRead(pinRedLed)? LOW:HIGH); 
 }

void DataLogger::toggleGreenLed(void)
 { digitalWrite(pinGreenLed, digitalRead(pinGreenLed)? LOW:HIGH); 
 }

void DataLogger::writeDigital(uint8_t p, bool s)   
 { uint8_t pin;
   if (p>3) 
      p=3; 
   pin = p+4;
   pinMode(pin, OUTPUT); 
   digitalWrite(pin, s ? HIGH : LOW); 
  }

bool DataLogger::readDigital(uint8_t p)            
 { uint8_t pin;
   if (p>3) 
      p=3; 
   pin = p+4;
   pinMode(pin, INPUT); 
   return digitalRead(pin)? HIGH : LOW; 
 }


uint16_t DataLogger::readRaw(uint8_t p) 
 { uint8_t pin;
   if (p>3) 
      p=3; 
   pin = p+14;
   return analogRead(pin); 
 } 

float DataLogger::readVoltage (uint8_t p, uint8_t nav, float ref)
 { long sum=0;
   uint8_t i;
         
   for (i=0; i<nav; i++)
      sum += readRaw(p); 
   uint16_t averagedRaw = sum/nav;
   
   return averagedRaw*ref/1024.0;
 }

