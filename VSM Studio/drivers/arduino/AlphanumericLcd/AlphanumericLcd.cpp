#pragma once

#include "AlphanumericLcd.h"

AlphanumericLcd::AlphanumericLcd(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
 : LiquidCrystal(rs,enable, d0, d1, d2, d3) { numrows=1, row=0; }    

void AlphanumericLcd::begin(uint8_t cols, uint8_t rows, uint8_t charsize)
// Overload to capture the number of rows.
 { LiquidCrystal::begin(cols,rows,charsize);
   numrows = rows;
 }

size_t AlphanumericLcd::write(uint8_t c)
// Overload this so that we can implement println.
 { if (c == '\n') 
      setCursor(0, row+1);
   else if (c != '\r') 
      LiquidCrystal::write(c);      
   return 1;
 }

   