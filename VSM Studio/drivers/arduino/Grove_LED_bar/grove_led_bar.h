#pragma once

#include <LED_bar.h>

class GroveLedBar : public LED_Bar
 { public:
      void setOrientation (bool greenToRed) { setGreenToRed(greenToRed); }
      void setLeds (unsigned int bits) { setBits(bits); }
      unsigned int getLeds (void)  { return getBits(); } 
 };
