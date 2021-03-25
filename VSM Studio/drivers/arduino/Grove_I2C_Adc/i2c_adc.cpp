/***************************************************************/
/****         Include files                                 ****/
/***************************************************************/

#include "i2c_adc.h"
#include <Wire.h>
#include <Arduino.h>

/***************************************************************/
/****         Class member functions                        ****/
/***************************************************************/

void I2C_ADC::begin(void)
 { // ax = 0 -> floating
   // ax = 1 -> GND
   // ax = 2 -> VCC
   uint8_t address;
   address = (a1<<4) | a0;
   switch (address)
    { case 0x00: _dvc_address = 0x50; break;
      case 0x01: _dvc_address = 0x51; break;
      case 0x02: _dvc_address = 0x52; break;
      case 0x10: _dvc_address = 0x54; break;
      case 0x11: _dvc_address = 0x55; break;
      case 0x12: _dvc_address = 0x56; break;
      case 0x20: _dvc_address = 0x58; break;
      case 0x21: _dvc_address = 0x59; break;
      case 0x22: _dvc_address = 0x5A; break;
    }
   Wire.begin();
 }
 
 
int I2C_ADC::read_i2c_adc(void)
 { return readTwoBytesReg(REG_CONVERSION_RESULT);
 }
 
float I2C_ADC::read_voltage(void)
 {
 
 }
 
/***************************************************************/
/****         Private functions                             ****/
/***************************************************************/

void I2C_ADC::writePointer(uint8_t reg)
 { Wire.beginTransmission(_dvc_address);
   Wire.write(reg);
   Wire.endTransmission();
 }

void I2C_ADC::writeOneByteReg(uint8_t reg, uint8_t data)
 { Wire.beginTransmission(_dvc_address);	 
   Wire.write(reg);	 
   Wire.write(data);	 
   Wire.endTransmission();	 
 }

uint8_t I2C_ADC::readOneByteReg(uint8_t reg)
 { uint8_t regValue;
   writePointer(reg);
   Wire.requestFrom(_dvc_address, 1);	 
   while(Wire.available())
    { 
      regValue = Wire.read();
    }
   return regValue;   
 }

void I2C_ADC::writeTwoBytesReg(uint8_t reg, uint16_t data)
 { uint8_t msb, lsb;
   msb = (data>>8) & 0xff;
   lsb = data & 0xff; 
   
   Wire.beginTransmission(_dvc_address);	 
   Wire.write(reg);	 
   Wire.write(msb);	 
   Wire.write(lsb);	 
   Wire.endTransmission();	 
 }

uint16_t I2C_ADC::readTwoBytesReg(uint8_t reg)
 { uint16_t u16data = 0;
   uint16_t u16array[2];
   uint8_t index = 0;
   writePointer(reg);
   
   Wire.requestFrom(_dvc_address, 2);
   while(Wire.available())
    { 
      u16array[index] = Wire.read();
      index++;
    }
   u16data = (u16array[0]<<8) | (u16array[1]);
   return u16data;
 }
 
  