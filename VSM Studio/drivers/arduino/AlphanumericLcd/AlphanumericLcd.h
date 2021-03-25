#pragma once

#include <core.h>
#include <liquidcrystal.h>

class AlphanumericLcd : public LiquidCrystal
 { public:
      AlphanumericLcd(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
      void begin(uint8_t cols, uint8_t rows, uint8_t charsize=LCD_5x8DOTS);
      void clear() { LiquidCrystal::clear(); row = 0; }
      void home() { LiquidCrystal::home(); row = 0; }
      void setCursor(uint8_t c, uint8_t r) { LiquidCrystal::setCursor(c, row=r); }
      PrintList &print() { return list.begin(false, this); }
      PrintList &println() { return list.begin(true, this); }
      void setPlaces(int p) { list.setPlaces(p); }
		void setBase(int b) { list.setBase(b); }
	private:
	   size_t write(uint8_t value);

   private:      
   	PrintList list;
   	uint8_t numrows, row;
 };
