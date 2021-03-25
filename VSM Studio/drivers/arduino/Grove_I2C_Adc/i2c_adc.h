#ifndef _I2C_ADC_H
#define _I2C_ADC_H

/***************************************************************/
/****         Include system files                          ****/
/***************************************************************/
#include <Wire.h>
#include <Arduino.h>

/***************************************************************/
/****         I2C ADC Registers                             ****/
/***************************************************************/
#define REG_CONVERSION_RESULT    0x00 
#define REG_ALERT_STATUS         0x01
#define REG_CONFIGURATION        0x02
#define REG_LOW_LIMIT            0x03
#define REG_HIGH_LIMIT           0x04
#define REG_HYSTERESIS           0x05
#define REG_LOWEST_CONVERSION    0x06
#define REG_HIGHEST CONVERSION   0x07

/***************************************************************/
/****         I2C_ADC Class                                 ****/
/***************************************************************/

class I2C_ADC
 { public:
      void begin();
      int read_i2c_adc();
      float read_voltage();   
      int a0, a1;
   private:
      void writePointer(uint8_t reg);
      void writeOneByteReg(uint8_t reg, uint8_t data);
      uint8_t readOneByteReg(uint8_t reg);
      void writeTwoBytesReg(uint8_t reg, uint16_t data);
      uint16_t readTwoBytesReg(uint8_t reg); 
      int _dvc_address;
 };

#endif  // _I2C_ADC_H