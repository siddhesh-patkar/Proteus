#pragma once

#include <Wire.h>
#include <TH02_dev.h>

class GroveTh02Dev : public TH02_dev
 { public:
      GroveTh02Dev () {};                        
      float readTemperature() { return ReadTemperature(); }
      float readRHumidity()  { return ReadHumidity(); }
 };
