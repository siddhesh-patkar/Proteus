// Generic Keypad scanner

#pragma once

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <Arduino.h>

class Keypad 
 { public: 
      Keypad (uint8_t r, uint8_t c, const char *t);
      void begin();
      bool operator()() { return scan() != 0; }
      String getKey (bool wait);
      void waitPress();
      void waitRelease();

   private:
      virtual void pinMode (uint8_t pin, uint8_t mode) = 0;
      virtual void digitalWrite (uint8_t pin, uint8_t state) = 0;
      virtual bool digitalRead(uint8_t pin) = 0;
      int scan();

   private:
      uint8_t numrows, numcols;
      const char *keytable;      
 };

#ifdef __MCP23008_H__

class Keypad_MCP23008 : public Keypad 
 { public:
      Keypad_MCP23008 (uint8_t a0, uint8_t a1, uint8_t a2, uint8_t rows, uint8_t cols, const char *keytable)
        : Keypad(rows, cols, keytable), io(a0, a1, a2) {}
      void begin() { io.begin(); Keypad::begin(); }
      
   private:
      void pinMode (uint8_t pin, uint8_t mode) 
       { if (mode == OUTPUT)
            io.pinMode(pin, OUTPUT); 
         else
            io.pinMode(pin, INPUT);
         if (mode == INPUT_PULLUP)
            io.pullUp(pin, HIGH);
        }

       void digitalWrite(uint8_t pin, uint8_t state) { io.digitalWrite(pin, state); }
       bool digitalRead(uint8_t pin) { return io.digitalRead(pin); }

   private:
      MCP23008 io;
 };


#endif


#endif