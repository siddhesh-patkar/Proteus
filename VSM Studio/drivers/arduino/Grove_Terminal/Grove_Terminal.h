#pragma once

#include <core.h>

class GroveTerminal
 { public:
      GroveTerminal() {} 
      void begin(unsigned long baud) { pinMode(1,OUTPUT); pinMode(0,INPUT); Serial.begin(baud, SERIAL_8N1); }                 
      StreamList &read() { return input.begin(&Serial); }
      String readln();
      PrintList &print() { return output.begin(false, &Serial); }
      PrintList &println() { return output.begin(true, &Serial); }
      void setDelimiter(char c) { input.setDelimiter(c); }
      void setPlaces(int p) { output.setPlaces(p); }
	   void setBase(int b) { output.setBase(b); }
   private:
   	StreamList input;
   	PrintList  output;
 };
