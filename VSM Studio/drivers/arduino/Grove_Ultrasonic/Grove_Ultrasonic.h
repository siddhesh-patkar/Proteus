#pragma once

#include <Ultrasonic.h>

class GroveUltrasonic : public Ultrasonic
 { public:
      void begin(int pin) { Ultrasonic_Init (pin);}
      int readCentimeters() { return MeasureInCentimeters(); }
      int readInches()  { return MeasureInInches(); }
 };
