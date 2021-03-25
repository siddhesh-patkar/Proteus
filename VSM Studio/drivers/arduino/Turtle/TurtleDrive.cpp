#include <arduino.h>
#include "Turtle.h"


void TurtleDrive::begin()
 { pinMode(in1, OUTPUT);
   pinMode(in2, OUTPUT);
   pinMode(ena, OUTPUT);
   pinMode(in3, OUTPUT);
   pinMode(in4, OUTPUT);
   pinMode(enb, OUTPUT);
 }

void TurtleDrive::drive(uint8_t wheel, uint8_t dir, uint8_t speed)
// Run the motor with specified direction and speed
 { bool isLeftWheelMoving = false;
   bool isRightWheelMoving = false;
   
   if (wheel & Left)
    { digitalWrite(in1, dir == Forwards ? HIGH : LOW);
      digitalWrite(in2, dir == Forwards ? LOW : HIGH);
      analogWrite(ena, speed);
      isLeftWheelMoving = speed > 0;
    }
   if (wheel & Right)
    { digitalWrite(in3, dir == Forwards ? HIGH : LOW);
      digitalWrite(in4, dir == Forwards ? LOW : HIGH);
      analogWrite(enb, speed);
      isRightWheelMoving = speed > 0;
    }
    
    isDriving = isLeftWheelMoving | isRightWheelMoving;
 }

void TurtleDrive::forwards(uint8_t speed)
 { drive(Both, Forwards, speed);
 }

void TurtleDrive::backwards(uint8_t speed)
 { drive(Both, Backwards, speed);
 }

void TurtleDrive::turn(int16_t speed)
 { if (speed > 0)
    { drive(Left, Backwards, speed);
      drive(Right, Forwards, speed);
    }
   else
    { drive(Left, Forwards, -speed);
      drive(Right, Backwards, -speed);
    }
 }

