/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****              Wrapper for Arduino Motor Shield V1          *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <arduino.h>
#include "motor_shield_v1.h"

/*********************************************************************
**** DC Motor ****
*****************/

void DCMotorV1::begin ()
 { pinMode(dir,OUTPUT);
   pinMode(pwm,OUTPUT);
   pinMode(brake,OUTPUT);      
   analogWrite(pwm, 0);
   digitalWrite(brake,LOW);
 }

void DCMotorV1::run (uint8_t direction, uint8_t speed)
// Run the motor with specified direction and speed
 { digitalWrite(brake, LOW); // Release the brake
   digitalWrite(dir, direction);
   analogWrite(pwm, speed);
 }
 
void DCMotorV1::stop ()
// Stop the motor with braking.
 { digitalWrite(brake, HIGH);
   analogWrite(pwm, 255); // full drive required for effective breaking   
 }

void DCMotorV1::release ()
// Stop the motor without braking (disengage the power)
 { analogWrite(pwm, 0);   
   digitalWrite(brake, LOW);
 }
 


/*********************************************************************
**** Stepper Motor ****
**********************/

void StepperMotorV1::begin ()
 { pinMode(dir1,OUTPUT);
   pinMode(pwm1,OUTPUT);
   pinMode(brake1,OUTPUT);      
   pinMode(dir2,OUTPUT);
   pinMode(pwm2,OUTPUT);
   pinMode(brake2,OUTPUT);      
   digitalWrite(pwm1, LOW);
   digitalWrite(pwm2, LOW);
   digitalWrite(brake1,LOW);
   digitalWrite(brake2,LOW);
   setSpeed(10);
 }
void StepperMotorV1::setSpeed(uint8_t rpm)
// Calculate us per step to be used when multi-stepping.
 { usperstep = 60000000 / ((uint32_t)revsteps * (uint32_t)rpm);   
 }
      
void StepperMotorV1::step (int16_t numsteps, uint8_t dir, uint8_t style)
// Execute one or more steps in the specified direction and style.
 { uint32_t uspers = usperstep;
   uint8_t ret = 0;

   if (style == INTERLEAVE) 
      uspers /= 2;
    
   while (numsteps--) 
    { oneStep(dir, style);
      if (uspers < 30000)
         delayMicroseconds(uspers);
      else   
         delay(uspers/1000);         
    }   
 }

void StepperMotorV1::oneStep (uint8_t dir, uint8_t style)
// Execute a single step.
// The supported styles are SINGLE,DOUBLE,INTERLEAVE (I don't think VSM can do MICROSTEP).
// SINGLE and DOUBLE both move one step at a time but DOUBLE drives two coils (in reverse polarity) at each step 
// Interleave gives the full resolution by alternating the drive.
 { if (style == SINGLE) 
    { if (currentstep % 2)     // we're at an odd step, weird
        if (dir == FORWARDS) 
           currentstep += 1;
        else 
   	     currentstep -= 1;
      else                     // Got to the next even step
         if (dir == FORWARDS) 
   	      currentstep += 2; 
         else 
   	      currentstep -= 2;
    }
   else if (style == DOUBLE)
    { if (! (currentstep % 2)) // we're at an even step, weird
         if (dir == FORWARDS) 
	         currentstep += 1;
         else
	         currentstep -= 1;
      else            // go to the next odd step
         if (dir == FORWARDS) 
	         currentstep += 2;
         else 
	         currentstep -= 2;
    }
   else if (style == INTERLEAVE) 
    { if (dir == FORWARDS) 
         currentstep += 1;
      else  
         currentstep -= 1;
    }
  
  
   // Keep the step number positive:
   currentstep += 8;
   currentstep %= 8;
  
   // Determine which coils to drive for the new step:
   uint8_t latch_state = 0; // all motor pins to 0

   switch (currentstep)
    { case 0: latch_state |= 0x9; break; // energize coil 1+4
      case 1: latch_state |= 0x1; break; // energize coil 1 only
      case 2: latch_state |= 0x3; break; // energize coil 1+2
      case 3: latch_state |= 0x2; break; // energize coil 2 only
      case 4: latch_state |= 0x6; break; // energize coil 2+3
      case 5: latch_state |= 0x4; break; // energize coil 3 only
      case 6: latch_state |= 0xC; break; // energize coil 3+4
      case 7: latch_state |= 0x8; break; // energize coil 4 only
    }

   // Set the coil drives for coils 1/2:
   // The logic here has to take account of the XOR gates:
   if (latch_state & 0x1) 
    { digitalWrite(dir1, HIGH);
      digitalWrite(brake1, (latch_state & 0x2) ? HIGH : LOW);
    }
   else 
    { digitalWrite(dir1, LOW);
      digitalWrite(brake1, (latch_state & 0x2) ? LOW : HIGH);
    }
   
   
   // Ditto coils 3/4:
   if (latch_state & 0x4) 
    { digitalWrite(dir2, HIGH);
      digitalWrite(brake2, (latch_state & 0x8) ? HIGH : LOW);
    }
   else 
    { digitalWrite(dir2, LOW);
      digitalWrite(brake2, (latch_state & 0x8) ? LOW : HIGH);
    }

   // Give it some welly:
   digitalWrite(pwm1, HIGH);
   digitalWrite(pwm2, HIGH);
          
}

   
void StepperMotorV1::release ()
// Disengage the drive coils.
 { digitalWrite(pwm1, LOW);
   digitalWrite(pwm1, LOW);
}
   
