#include <Arduino.h>
#include "Grove_Dust.h"

static uint8_t pin = 8;
static volatile long lowpulsestart = 0;
static volatile long lowpulseoccupancy = 0;


ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
 {   if (digitalRead(pin) == 0)
         lowpulsestart = millis();
     else if (lowpulsestart != 0)
         lowpulseoccupancy += (long)(millis()-lowpulsestart);
 } 


GroveDustSensor::GroveDustSensor()
 { readings = NULL;
   setSamplePeriod(60);
   setNumSamples(10);
 }

void GroveDustSensor::begin (int p)
 { // Enable the pin change interrupt
   pinMode(p, INPUT_PULLUP);
   *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
   PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
   PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
 }
   
void GroveDustSensor::poll()
 { long now = millis();
   if ((long)(now-nextsample) >= 0)
    {  noInterrupts();
       float ratio = 100.0*lowpulseoccupancy/(sampleperiod);  // Integer percentage 0=>100
       lowpulseoccupancy = 0;
       interrupts();
       concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
       readings[reading++] = concentration;
       reading %= numsamples;
       nextsample += sampleperiod;
    }    
 }        

void GroveDustSensor::setSamplePeriod (int p)
 { sampleperiod = p*1000L;  
   nextsample = millis()+sampleperiod;
 }

void GroveDustSensor::setNumSamples (int ns)           
 { readings = (long *)realloc(readings, ns*sizeof(long));
   numsamples = ns;   
   for (int i=0; i<numsamples; ++i)
      readings[i] = 0;
   reading = 0;
 }
   
long GroveDustSensor::readDustAvg ()
 { long avg = 0;
   for (int i=0; i<numsamples; ++i)
      avg += readings[i];
   return avg/numsamples;      
 }
