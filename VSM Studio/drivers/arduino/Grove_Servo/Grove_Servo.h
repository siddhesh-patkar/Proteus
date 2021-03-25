#pragma once

#include <Servo.h> 

class GroveServo : public Servo
 { public:
      void begin(uint8_t pin, float minf, float maxf) { Servo::attach(pin, min=minf*1000000, maxf*1000000); }
      bool operator() () { return attached(); }
      float read() { return Servo::read(); }
      void write(float angle) { Servo::write(angle); }
      uint16_t readMicroseconds() { return Servo::readMicroseconds(); }      
      void writeMicroseconds(uint16_t time) { Servo::writeMicroseconds(time); }
      void attach()  { if (!attached()) Servo::attach(pin, min, max); }
      void detach() { if (attached()) Servo::detach(); }
            
   private:
      uint8_t pin;
      uint16_t min, max;
 };