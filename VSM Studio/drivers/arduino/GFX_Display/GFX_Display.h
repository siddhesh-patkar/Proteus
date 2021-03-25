// Wrapper for Graphics Displays under Visual Designer


#ifndef __GFX_DISPLAY_H__
#define __GFX_DISPLAY_H__

#include <core.h>

#include "utility\Adafruit_ILI9341.h"
#include "utility\Adafruit_ST7735.h"
#include "utility\Grove_ssd1308.h"
#include "utility\Adafruit_PCD8544.h"

template<class T>
class GFX_Display : public T
 { public:
      GFX_Display () {} 
      GFX_Display (int8_t _DC, int8_t _CS, int8_t _RST) : T(_DC, _CS, _RST) {}
      GFX_Display (int8_t _CS, int8_t _DC) : T(_CS, _DC) {}
 
      // Visual designer extensions:
      void setTextBackground(uint16_t bgc) { setTextColor(this->Adafruit_GFX::textcolor, bgc); }
      void drawBitmap (const String &s) { drawBitmap(s.c_str()); }
      #if defined(__SD_H__)
      void drawBitmap (const char *resource, int16_t x, int16_t y)
       { PImage img = Adafruit_GFX::loadImage(resource);
         Adafruit_GFX::image(img, x, y);
         img.close();
       }   
      #else
      void drawBitmap (const char *resource, int16_t x, int16_t y) {}
      #endif

      PrintList &print() { return list.begin(false, this); }
      PrintList &println() { return list.begin(true, this); }
      void setPlaces(int p) { list.setPlaces(p); }
      void setBase(int b) { list.setBase(b); }
   private:
  	   PrintList list;
 
 };

#endif
