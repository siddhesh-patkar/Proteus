#pragma once

#include <Adafruit_NeoPixel.h>


class NeoPixel : public Adafruit_NeoPixel
 { public:
      NeoPixel (uint16_t n, uint8_t p) : Adafruit_NeoPixel(n, p) {};


 };
