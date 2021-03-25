#include <arduino.h>
#include "Turtle.h"


void TurtleLineHunter::begin()
 { pinMode(pin_left, INPUT);
   pinMode(pin_centre, INPUT);
   pinMode(pin_right, INPUT);
 }

bool TurtleLineHunter::operator() (int8_t left, int8_t centre, int8_t right)
 { bool lf = left   == -1 ? true  : left   == digitalRead(pin_left);
 	 bool cf = centre == -1 ? true  : centre == digitalRead(pin_centre);
 	 bool rf = right  == -1 ? true  : right  == digitalRead(pin_right);
 	 return lf && cf && rf;
 }
