#pragma once

#include <inttypes.h>
#include <Wire.h>
#include <Servo.h> 
#include "Adafruit_PWMServoDriver.h"

#define FORWARD 1
#define BACKWARD 0

class Adafruit_MotorShield : public Adafruit_PWMServoDriver
 { public:
      void begin(uint8_t addr);
      void setPWM(uint8_t pin, uint16_t val);
      void setPin(uint8_t pin, boolean val);
   private:
      uint8_t _addr;
      Adafruit_PWMServoDriver _pwm;
 };

class DCMotorV2
 { public:
      DCMotorV2 (uint8_t ch) : channel(ch), running(false) {}
      void begin(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);

      bool operator() () { return running; }
      
      void run (uint8_t dir, uint8_t speed=255);
      void release ();
       
   private:
      uint8_t pwm, in1, in2;
      uint8_t channel;
      bool running;
      Adafruit_MotorShield *MotorController;
 };

// Stepper Motor Object
#define SINGLE 1
#define DOUBLE 2
#define INTERLEAVE 3
#define MICROSTEP 4

class StepperMotorV2
 { public:
      StepperMotorV2 (uint8_t ch, uint16_t s) : channel(ch), revsteps(360/s), currentstep(0) {}
      void begin(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5);
      void setSpeed(uint16_t rpm);
      void step (uint16_t numsteps, uint8_t dir, uint8_t style);
      void oneStep (uint8_t dir, uint8_t style);
      void release ();
  
   private:
      uint8_t pwma, ain1, ain2;
      uint8_t pwmb, bin1, bin2;
      uint8_t channel;
      uint16_t revsteps; // # steps per revolution
      uint8_t currentstep;
      uint32_t usperstep;     
      Adafruit_MotorShield *MotorController;
};
 

class ServoMotorV2 : public Servo
 { public:
      void begin(uint8_t pin, float minf, float maxf) { Servo::attach(pin, min=minf*1000000, maxf*1000000); }
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




