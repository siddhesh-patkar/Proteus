#pragma once

#include <Wire.h>
#include <FDC1004.h>

#ifndef __CAPDAC_H__
#define __CAPDAC_H__

class CapDac : public FDC1004
 { public:
      CapDac () {};
      int32_t readCapacitance(uint8_t channel = 1) { return getCapacitance(channel); }
      int32_t readRowData(uint8_t channel, uint8_t capdac) { uint16_t value[2];
                                                             FDC1004::measureChannel(channel, capdac, value); 
                                                             return  ((int32_t)(value[0])<<8) + ((int32_t)(value[1])>>8);
                                                           } 
      
      int32_t readMeasureReg(uint8_t measure)              { uint16_t value[2];
                                                             FDC1004::readMeasRegisters(measure, value); 
                                                             return  ((int32_t)(value[0])<<8) + ((int32_t)(value[1])>>8);
                                                           } 
 };
 
#endif
