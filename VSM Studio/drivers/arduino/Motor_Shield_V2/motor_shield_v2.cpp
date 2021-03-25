/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****              Wrapper for Arduino Motor Shield V2         *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/
#include <arduino.h>
#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"
#include "motor_shield_v2.h"

/*********************************************************************
**** MotorShield helper class ****
*****************^***************/

void Adafruit_MotorShield::begin(uint8_t addr)
 { // Initialize and reset wire interface.
   _pwm.begin();
   // Pass I2C address to PWM Servo driver.
   _pwm = Adafruit_PWMServoDriver(addr);
 }

void Adafruit_MotorShield::setPWM(uint8_t pin, uint16_t value) {
  if (value > 4095) {
    _pwm.setPWM(pin, 4096, 0);
  } else 
    _pwm.setPWM(pin, 0, value);
}
void Adafruit_MotorShield::setPin(uint8_t pin, boolean value) {
  if (value == LOW)
    _pwm.setPWM(pin, 0, 0);
  else
    _pwm.setPWM(pin, 4096, 0);
}

/*********************************************************************
**** DC Motor ****
*****************/

void DCMotorV2::begin (uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5) 
 { // ax = 0 => FLT
   // ax = 1 => GND
   // ax = 2 => VCC
   a0 = (a0==0)? 1 : (a0==2)? 1 : 0;
   a1 = (a1==0)? 1 : (a1==2)? 1 : 0; 
   a2 = (a2==0)? 1 : (a2==2)? 1 : 0; 
   a3 = (a3==0)? 1 : (a3==2)? 1 : 0; 
   a4 = (a4==0)? 1 : (a4==2)? 1 : 0; 
   a5 = (a5==0)? 1 : (a5==2)? 1 : 0; 
   uint8_t addr = (a5<<5) | (a4<<4) | (a3<<3) | (a2<<2) | (a1<<1) | a0;
   
   // Transfer computed I2C address plus base address to MotorShield class. 
   MotorController->begin(0x40+addr);
   
   switch (channel)
    { case 1:  pwm =  8; in2 =  9; in1 = 10; break;
      case 2:  pwm = 13; in2 = 12; in1 = 11; break; 
      case 3:  pwm =  2; in2 =  3; in1 =  4; break;
      case 4:  pwm =  7; in2 =  6; in1 =  5; break; 
      default: pwm =  8; in2 =  9; in1 = 10; break;
    }
 }

// Run the motor with specified direction and speed.
void DCMotorV2::run (uint8_t direction, uint8_t speed)
 { // Set speed:
   MotorController->setPWM(pwm, speed*16);
   running = true;
   
   // Set direction: 
   switch (direction) 
    { case FORWARD:
         MotorController->setPin(in2, LOW);  // take low first to avoid 'break'
         MotorController->setPin(in1, HIGH);
         break;
      case BACKWARD:
         MotorController->setPin(in1, LOW);  // take low first to avoid 'break'
         MotorController->setPin(in2, HIGH);
         break;
    }
 }

void DCMotorV2::release ()
// Stop the motor without braking (disengage the power)
 { MotorController->setPin(in1, LOW);
   MotorController->setPin(in2, LOW);
   running = false;
 }

/*********************************************************************
**** Stepper Motor ****
**********************/
#define MICROSTEPS 8
uint8_t microstepcurve[] = {0, 50, 98, 142, 180, 212, 236, 250, 255};

void StepperMotorV2::begin (uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5)
 { // ax = 0 => FLT
   // ax = 1 => GND
   // ax = 2 => VCC
   a0 = (a0==0)? 1 : (a0==2)? 1 : 0;
   a1 = (a1==0)? 1 : (a1==2)? 1 : 0; 
   a2 = (a2==0)? 1 : (a2==2)? 1 : 0; 
   a3 = (a3==0)? 1 : (a3==2)? 1 : 0; 
   a4 = (a4==0)? 1 : (a4==2)? 1 : 0; 
   a5 = (a5==0)? 1 : (a5==2)? 1 : 0; 
   uint8_t addr = (a5<<5) | (a4<<4) | (a3<<3) | (a2<<2) | (a1<<1) | a0;
   
   // Transfer computed I2C address plus base address to MotorShield class. 
   MotorController->begin(0x40+addr);
 
   switch (channel)
    { case 1:  pwma = 8; ain2 = 9; ain1 = 10; pwmb = 13; bin2 = 12; bin1 = 11; break;
      case 2:  pwma = 2; ain2 = 3; ain1 =  4; pwmb =  7; bin2 =  6; bin1 =  5; break;
      default: pwma = 8; ain2 = 9; ain1 = 10; pwmb = 13; bin2 = 12; bin1 = 11; break;
    }
 }

void StepperMotorV2::setSpeed(uint16_t rpm) 
 { // Compute time in us per step.
   usperstep = 60000000 / ((uint32_t)revsteps * (uint32_t)rpm);
 }

void StepperMotorV2::release(void) 
 { 
   MotorController->setPin(ain1, LOW);
   MotorController->setPin(ain2, LOW);
   MotorController->setPin(bin1, LOW);
   MotorController->setPin(bin2, LOW);
   MotorController->setPWM(pwma, 0);
   MotorController->setPWM(pwmb, 0);
}

void StepperMotorV2::step(uint16_t steps, uint8_t dir, uint8_t style) 
 { uint32_t uspers = usperstep;

   if (style == INTERLEAVE) 
    { uspers /= 2;
    }
   else if (style == MICROSTEP) 
    { uspers /= MICROSTEPS;
      steps *= MICROSTEPS;
    }

   while (steps--) 
    { oneStep(dir, style);
      delayMicroseconds(uspers);
    }
 }

void StepperMotorV2::oneStep(uint8_t dir, uint8_t style) 
 { uint8_t ocrb, ocra;
   ocra = ocrb = 255;

   // Next determine what sort of stepping procedure we're up to
   if (style == SINGLE) 
    { if ((currentstep/(MICROSTEPS/2)) % 2) 
       { // We're at an odd step, weird
         if (dir == FORWARD) 
            currentstep += MICROSTEPS/2;
         else 
            currentstep -= MICROSTEPS/2;
       } 
      else 
       { // go to the next even step
         if (dir == FORWARD) 
          { currentstep += MICROSTEPS;
          }
         else 
          { currentstep -= MICROSTEPS;
          }
       }
    } 
   else if (style == DOUBLE) 
    { if (! (currentstep/(MICROSTEPS/2) % 2)) 
       { // we're at an even step, weird
         if (dir == FORWARD) 
            currentstep += MICROSTEPS/2;
         else 
            currentstep -= MICROSTEPS/2;
       } 
      else 
       { // go to the next odd step
         if (dir == FORWARD) 
            currentstep += MICROSTEPS;
         else 
            currentstep -= MICROSTEPS;
       }
    }  
   else if (style == INTERLEAVE) 
    { if (dir == FORWARD)
         currentstep += MICROSTEPS/2;
      else 
         currentstep -= MICROSTEPS/2;
    } 

   if (style == MICROSTEP) 
    { if (dir == FORWARD) 
         currentstep++;
      else 
         currentstep--;

      currentstep += MICROSTEPS*4;
      currentstep %= MICROSTEPS*4;

      ocra = ocrb = 0;
      if ( (currentstep >= 0) && (currentstep < MICROSTEPS)) 
       { ocra = microstepcurve[MICROSTEPS - currentstep];
         ocrb = microstepcurve[currentstep];
       } 
      else if  ( (currentstep >= MICROSTEPS) && (currentstep < MICROSTEPS*2)) 
       { ocra = microstepcurve[currentstep - MICROSTEPS];
         ocrb = microstepcurve[MICROSTEPS*2 - currentstep];
       } 
      else if  ( (currentstep >= MICROSTEPS*2) && (currentstep < MICROSTEPS*3)) 
       { ocra = microstepcurve[MICROSTEPS*3 - currentstep];
         ocrb = microstepcurve[currentstep - MICROSTEPS*2];
       } 
      else if  ( (currentstep >= MICROSTEPS*3) && (currentstep < MICROSTEPS*4)) 
       { ocra = microstepcurve[currentstep - MICROSTEPS*3];
         ocrb = microstepcurve[MICROSTEPS*4 - currentstep];
       }
    }

   currentstep += MICROSTEPS*4;
   currentstep %= MICROSTEPS*4;

   MotorController->setPWM(pwma, ocra*16);
   MotorController->setPWM(pwmb, ocrb*16);

   // Release all motor pins to 0
   uint8_t latch_state = 0;

   if (style == MICROSTEP) 
    { if ((currentstep >= 0) && (currentstep < MICROSTEPS))
         latch_state |= 0x03;
      if ((currentstep >= MICROSTEPS) && (currentstep < MICROSTEPS*2))
         latch_state |= 0x06;
      if ((currentstep >= MICROSTEPS*2) && (currentstep < MICROSTEPS*3))
         latch_state |= 0x0C;
      if ((currentstep >= MICROSTEPS*3) && (currentstep < MICROSTEPS*4))
         latch_state |= 0x09;
    } 
   else 
    { switch (currentstep/(MICROSTEPS/2)) 
       { case 0:
            latch_state |= 0x1; // energize coil 1 only
            break;
         case 1:
            latch_state |= 0x3; // energize coil 1+2
            break;
         case 2:
            latch_state |= 0x2; // energize coil 2 only
            break;
         case 3:
            latch_state |= 0x6; // energize coil 2+3
            break;
         case 4:
            latch_state |= 0x4; // energize coil 3 only
            break; 
         case 5:
            latch_state |= 0xC; // energize coil 3+4
            break;
         case 6:
            latch_state |= 0x8; // energize coil 4 only
            break;
         case 7:
            latch_state |= 0x9; // energize coil 1+4
            break;
       }
    }

   if (latch_state & 0x1) 
      MotorController->setPin(ain2, HIGH);
   else
      MotorController->setPin(ain2, LOW);
  
   if (latch_state & 0x2)
      MotorController->setPin(bin1, HIGH);
   else
      MotorController->setPin(bin1, LOW);

   if (latch_state & 0x4)
      MotorController->setPin(ain1, HIGH);
   else
      MotorController->setPin(ain1, LOW);

   if (latch_state & 0x8)
      MotorController->setPin(bin2, HIGH);
   else
      MotorController->setPin(bin2, LOW);
 }

