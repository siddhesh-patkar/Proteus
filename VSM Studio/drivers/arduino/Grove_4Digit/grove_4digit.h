#pragma once

#include <TM1637.h>

class Grove4DigitDisplay : public TM1637
 { public:
      void setBrightness(uint8_t b) { set(b); }
      void decPoint(bool s) { point(s ? 1 : 0); }  
 };
