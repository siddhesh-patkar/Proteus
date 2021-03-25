#pragma once

#define FORWARDS  1
#define BACKWARDS 0


class DCMotorV1 
 { public:
      DCMotorV1 (uint8_t d, uint8_t p, uint8_t b) : dir(d), pwm(p), brake(b), running(false) {}
      void begin();
      
      bool operator() () { return running; }
      
      void run (uint8_t dir, uint8_t speed=255);
      void stop();
      void release ();
            
   private:
      uint8_t dir, pwm, brake;
      bool running;
      
};

// Stepper Motor Object
#define SINGLE 1
#define DOUBLE 2
#define INTERLEAVE 3
#define MICROSTEP 4

class StepperMotorV1 
 { public:
      StepperMotorV1 (uint8_t d1, uint8_t p1, uint8_t b1, uint8_t d2, uint8_t p2, uint8_t b2, uint8_t s) 
        : dir1(d1), pwm1(p1), brake1(b1), dir2(d2), pwm2(p2), brake2(b2), revsteps (360/s), currentstep(0) {}
      void begin ();
      void setSpeed(uint8_t rpm);
      void step (int16_t numsteps, uint8_t dir, uint8_t style);
      void oneStep (uint8_t dir, uint8_t style);
      void release ();
      
   private:
      uint8_t dir1, pwm1, brake1;
      uint8_t dir2, pwm2, brake2;
      uint16_t revsteps;
      uint16_t currentstep;
      uint32_t usperstep;


 };


