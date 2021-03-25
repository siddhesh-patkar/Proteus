#pragma once

#include <Servo.h>

class TurtleDrive
 { public:
      enum Direction
       { Backwards = 0,
         Forwards = 1
       };
       
      enum Wheel
       { Left   = 1,
         Right  = 2,
         Both   = Left | Right
       };

   public:
      TurtleDrive(uint8_t _in1,
                  uint8_t _in2,
                  uint8_t _ena,
                  uint8_t _in3,
                  uint8_t _in4,
                  uint8_t _enb)
          : in1(_in1)
          , in2(_in2)
          , ena(_ena)
          , in3(_in3)
          , in4(_in4)
          , enb(_enb)
          , isDriving(false)
       {
       }

      bool operator() () { return isDriving; }
      void begin();
      
      void drive(uint8_t wheel=Both, uint8_t dir=Forwards, uint8_t speed=255);
      void forwards(uint8_t speed=255);
      void backwards(uint8_t speed=255);
      void turn(int16_t speed=255);
      void stop() { forwards(0); }
            
   private:
      uint8_t in1, in2, ena;
      uint8_t in3, in4, enb;
      bool isDriving;
};

class TurtleSonarHead : public Servo
 { public:
      TurtleSonarHead(uint8_t _sonar_trig, uint8_t _sonar_echo, uint8_t _servo) : sonar_trig(_sonar_trig), sonar_echo(_sonar_echo), servo(_servo) {}

      void begin();
      bool operator() (float distance, float angle);

      void setAngle(float angle);
      void setRange(float cm);
      float ping();
      
   private:
      uint8_t sonar_trig, sonar_echo, servo;
      uint16_t pingTime;
 };
 
class TurtleLineHunter
 { public:
      TurtleLineHunter(uint8_t _pin_left, uint8_t _pin_centre, uint8_t _pin_right) : pin_left(_pin_left), pin_centre(_pin_centre), pin_right(_pin_right) {}

      void begin();
      bool operator() (int8_t left, int8_t centre, int8_t right);

   private:
      uint8_t pin_left;
      uint8_t pin_centre;
      uint8_t pin_right;
 };   
 