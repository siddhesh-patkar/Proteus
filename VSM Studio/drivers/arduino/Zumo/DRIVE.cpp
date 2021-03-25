#include <arduino.h>

#include "Zumo.h"

DRIVE *DRIVE::instance = 0;
DRIVE *DRIVE::getInstance()
 { return instance;
 }

void DRIVE::begin()
 { instance = this;
   
   pinMode(dir_l, OUTPUT);
   pinMode(spd_l, OUTPUT);
   pinMode(dir_r, OUTPUT);
   pinMode(spd_r, OUTPUT);
 }

void DRIVE::drive(uint8_t wheel, uint8_t dir, uint8_t speed)
// Run the motor with specified direction and speed
 { bool isLeftWheelMoving = false;
   bool isRightWheelMoving = false;
   
   if (wheel & Left)
    { digitalWrite(dir_l, dir == Forwards ? LOW : HIGH);
      analogWrite(spd_l, speed);
      isLeftWheelMoving = speed > 0;
    }
   if (wheel & Right)
    { digitalWrite(dir_r, dir == Forwards ? LOW : HIGH);
      analogWrite(spd_r, speed);
      isRightWheelMoving = speed > 0;
    }
    
    isDriving = isLeftWheelMoving | isRightWheelMoving;
 }

void DRIVE::forwards(uint8_t speed)
 { drive(Both, Forwards, speed);
 }

void DRIVE::backwards(uint8_t speed)
 { drive(Both, Backwards, speed);
 }

void DRIVE::turn(int16_t speed)
 { if (speed > 0)
    { drive(Left, Forwards, speed);
      drive(Right, Backwards, speed);
    }
   else
    { drive(Left, Backwards, -speed);
      drive(Right, Forwards, -speed);
    }
 }

void DRIVE::turnDegrees(int16_t degrees, int16_t speed)
 { if (speed == 0)
      return;
   turn(speed);
   
   // read the angular speed
   GYRO *gyro = GYRO::getInstance();
   if (gyro)
    {
      float gyroX, gyroY, gyroZ;
      gyro->readAngularAcc(&gyroX, &gyroY, &gyroZ);
      
      int totalDelay = 0;
      int delayStep = 10;
      while (gyroZ == 0.0)
       { delay(delayStep);
         totalDelay += delayStep;
         gyro->readAngularAcc(&gyroX, &gyroY, &gyroZ);
         
         if (totalDelay > 1000) // break eternal loop if something went wrong
            break;
       }

      if (gyroZ != 0.0)       
       { // gyroZ is the angular speed in deg/sec so work out how long we need to turn for
         float duration = (float)degrees / gyroZ * 1000.0f;
         if (duration < 0.0f)
            duration = -duration;
         delay((int)duration - totalDelay);
       }
    }
   
   stop();
 }

 
 