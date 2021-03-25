#pragma once

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class PwmServoController : public Adafruit_PWMServoDriver
 { public:
      PwmServoController (uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5) 
       { // ax = 0 => FLT
         // ax = 1 => GND
         // ax = 2 => VCC
         a0 = (a0==0)? 1 : (a0==2)? 1 : 0;
         a1 = (a1==0)? 1 : (a1==2)? 1 : 0; 
         a2 = (a2==0)? 1 : (a2==2)? 1 : 0; 
         a3 = (a3==0)? 1 : (a3==2)? 1 : 0; 
         a4 = (a4==0)? 1 : (a4==2)? 1 : 0; 
         a5 = (a5==0)? 1 : (a5==2)? 1 : 0; 
         a = (a5<<5) | (a4<<4) | (a3<<3) | (a2<<2) | (a1<<1) | a0;
       };                        
      
      void resetPWM(void) { reset(); }
};
