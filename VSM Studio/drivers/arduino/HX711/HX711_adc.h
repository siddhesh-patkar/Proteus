#pragma once

#include <HX711.h>

#ifndef __HX711_H__
#define __HX711_H__

class HX711_adc : public HX711
 { public:
      HX711_adc (byte dout, byte pd_sck, byte gain = 128) : HX711(dout, pd_sck, gain){} ;
 
      void writeGain(byte gain = 128)        { HX711::set_gain(gain); }
      void writeScale(float scale = 1.f)     { HX711::set_scale(scale); }
      void writeTare(byte times = 10)        { HX711::tare(times); }
      void powerDown(void)                   { HX711::power_down(); }
      void powerUp(void)                     { HX711::power_up(); }
      
      long readAverage(byte times = 10)      { return HX711::read_average(times); }
      long readTare(void)                    { return HX711::get_offset(); }
      double readValue(byte times = 1)       { return HX711::get_value(times); }
      float readUnits(byte times = 1)        { return HX711::get_units(times); }
      float readScale(void)                  { return HX711::get_scale(); }                  
 };
 
#endif
