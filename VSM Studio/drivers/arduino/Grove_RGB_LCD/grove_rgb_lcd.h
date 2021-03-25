#pragma once

#include <core.h>
#include <wire.h>
#include <rgb_lcd.h>

class GroveRgbLcd : public rgb_lcd
 { public:
      GroveRgbLcd () {}
      PrintList &print() { return list.begin(false, this); }
      PrintList &println() { return list.begin(true, this); }
      void setPlaces(int p) { list.setPlaces(p); }
		  void setBase(int b) { list.setBase(b); }
   private:
   	  PrintList list;
 };
