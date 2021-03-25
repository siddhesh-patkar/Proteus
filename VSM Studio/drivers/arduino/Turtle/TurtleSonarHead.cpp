#include <arduino.h>
#include "Turtle.h"


void TurtleSonarHead::begin()
 { pinMode(sonar_trig, OUTPUT);
   pinMode(sonar_echo, INPUT);
   pinMode(servo, OUTPUT);

   Servo::attach(servo);
   	
   setRange(100);
 }

bool TurtleSonarHead::operator() (float distance, float angle)
 { // set servo to angle and wait until it gets there
   Servo::write(angle + 90.0f);
   float angleToMove = fabs(Servo::read() - 90.0f);
   const float degreesPerSecond = 360.0f;
   delay(angleToMove * degreesPerSecond * 1000.0f);
   
   // ping for distance   
   float range = ping();
	 
   return range > 0 && range <= distance;
 }

void TurtleSonarHead::setAngle(float angle)
 { Servo::write(angle + 90.0f);
 }
 
void TurtleSonarHead::setRange(float range)
 { if (range < 10)
 			range = 10;
 	 else if (range > 400)
 	 		range = 400;		
 	 pingTime = range*58.0;
 } 	
	 
 
float TurtleSonarHead::ping()
 { long duration1=0, duration2=0;
   delay(1);
	 digitalWrite(sonar_trig, LOW);
   delayMicroseconds(2);
   digitalWrite(sonar_trig, HIGH);
   delayMicroseconds(10);
	 digitalWrite(sonar_trig, LOW);
	 duration1 = pulseIn(sonar_echo, HIGH, pingTime);
   delay(1);
   digitalWrite(sonar_trig, LOW);
   delayMicroseconds(2);
   digitalWrite(sonar_trig, HIGH);
   delayMicroseconds(10);
	 digitalWrite(sonar_trig, LOW);
	 duration2 = pulseIn(sonar_echo, HIGH, pingTime);
	 return duration1 != 0 && duration2 != 0 ? (duration1+duration2) / (2*58.0f) : -1;
 }
