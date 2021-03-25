
#include "Sparkfun_Weather.h"
#include "vsm.h"

/*********************************************************************
**** Initialization and Polling ****
***********************************/

WeatherStation::WeatherStation (uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5)
              : wdout(p1), wsout(p2), rgout(p3), lsref(p4), lsout(p5)  {}

void WeatherStation::begin ()
 { // Initialize pin modes:
   pinMode(wdout, INPUT);        // input from wind meters direction sensor
   pinMode(wsout, INPUT_PULLUP); // input from wind meters windspeed sensor
   pinMode(rgout, INPUT_PULLUP); // input from wind meters rain gauge sensor
   pinMode(lsref, INPUT); // light sensor voltage ref
   pinMode(lsout, INPUT); // ligh sensor output
   
   // Initialize the pressure/temperature sensor:
   pressure.begin();
   pressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
   pressure.setOversampleRate(7); // Set Oversample to the recommended 128
   pressure.enableEventFlags(); // Enable all three pressure and temp event flags
   
   // Initialize the humidity sensor:
   humidity.begin();

   // Attach interrupts to the rain and pressure sensors on IO2/IO3.   
   attachInterrupt(digitalPinToInterrupt(rgout), rainIRQ, FALLING);
   attachInterrupt(digitalPinToInterrupt(wsout), wspeedIRQ, FALLING);
   interrupts();
      
   // Reset all the counters and statistics:
   reset();
 }


void WeatherStation::poll ()   
 { // The weather station code is automatically polled in order to update all the local variables and history values.
   unsigned long now = millis(); // grab current time
   if (now > nextpoll)
    { // The processing here is done with interrupts disabled to ensure integrity of the values:
      noInterrupts();
      windspeedavg[second] = rawWindSpeed();
      winddiravg[second] = rawWindDirection();
      windticks = 0;
      raintenminutes += rainticks*0.2794;
      rainticks = 0;
      interrupts();
    
      // Capture the any higher wind gust for this minute.
      byte minute_10 = minute % 10;
      if (windspeedavg[second] > windgust_10m[minute_10])
       { windgust_10m[minute_10] = windspeedavg[second];
         windgustdir_10m[minute_10] = winddiravg[second];
       }  
       
      // Clock the time counters, and reset
      lastpoll = now, nextpoll += 1000;
      if (++second == 60)
       { // Update the rainfall numbers for the current hour and day:
         if (minute % 10 == 0)
          { rainhour[minute/10] = raintenminutes*100;
            raintenminutes = 0;
          }
         rainday[hour] = getRainHour()*10;            
         
         // Clock the minutes and hours:
         if (++minute == 60)
          { if (++hour == 24)
               hour = 0;
            minute = 0;
          }
         second = 0;

         // Reset the peak wind gust and rainfall for the new minute
         windgust_10m[minute % 10] = 0; 
       }  
    }
 }
 
/*********************************************************************
**** User API Functions  ****
****************************/
 
void WeatherStation::reset ()
 { // Reset the time counters:
   lastpoll = millis();
   nextpoll = lastpoll+1000;
   second = minute = hour = 0;   
   
   // Reset wind data:
   for (int i=0; i<60; ++i)
    { windspeedavg[i] = 0;
      winddiravg[i] = 0;
    }
   for (int i=0; i<10; ++i)
    { windgust_10m[i] = 0;
      windgustdir_10m[i] = 0;
    }
    
   // Reset rainfall data: 
   raintenminutes = 0; 
   for (int i=0; i<6; ++i)
      rainhour[i] = 0;
   for (int i=0; i<24; ++i)
      rainday[i] = 0;          

   // Reset the interrupt driven counters:
   noInterrupts();
   windticks = 0;
   rainticks = 0;
   interrupts();
 }
  
float WeatherStation::getTemperature (void)
 { return pressure.readTemp();
 }

float WeatherStation::getPressure (void)
 { return pressure.readPressure() / 100.0;   // convert Pa to mBar.
 }

float WeatherStation::getHumidity (void)
 { return humidity.getRH();
 }   
      
float WeatherStation::getDaylight()
// Return a daylight value 0->1
 { float operatingVoltage = analogRead(lsref);
   float lightSensor = analogRead(lsout);
   operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
   lightSensor = operatingVoltage * lightSensor / 1024;
   return lightSensor;
 }
      
float WeatherStation::getWindSpeed ()
// Compute the average wind speed for the last minute:
 { float temp = 0;
   for (int i = 0 ; i < 60 ; i++)
      temp += windspeedavg[i];
   return temp / 60.0;
 }

float WeatherStation::getWindDirection ()
// Compute the average wind direction for the last minute.
 { // You can't just take the average. Google "mean of circular quantities" for more info
   // We will use the Mitsuta method because it doesn't require trig functions
   // And because it sounds cool.
   // Based on: http://abelian.org/vlf/bearings.html
   // Based on: http://stackoverflow.com/questions/1813483/averaging-angles-again
   long sum = winddiravg[0];
   int D = winddiravg[0];
   for (int i = 1 ; i < 60; i++)
    { int delta = winddiravg[i] - D;
      if (delta < -180)
         D += delta + 360;
      else if (delta > 180)
         D += delta - 360;
      else
         D += delta;
      sum += D;
    }
    
   float winddir_avg = sum / 60.0;
   if(winddir_avg >= 360) winddir_avg -= 360;
   if(winddir_avg < 0) winddir_avg += 360;
   return winddir_avg;
 }
 
float WeatherStation::getGustSpeed()
// Look for the highest wind gust in the last 10 minutes.   
{ float gust = 0;
  for (int i = 0; i < 10 ; i++)
     if (windgust_10m[i] > gust)
        gust = windgust_10m[i];
   return gust;
 }
    
float WeatherStation::getGustDirection() 
// Look for the highest wind gust in the last 10 minutes and return its direction.
{ float gust = 0, dir = 0;
  for (int i = 0; i < 10 ; i++)
     if (windgust_10m[i] > gust)
      { gust = windgust_10m[i];
        dir = windgustdir_10m[i];
      }
   return dir;
 }


float WeatherStation::getRainHour()
// Compute rainfall for the last hour (rolling value).
 { float rain = 0;
   for (int i=0; i<6; ++i)
      rain += rainhour[i];
   return rain/100;   
 }   

float WeatherStation::getRainDay()
// Compute rainfall for the last hour (rolling value).
 { float rain = 0;
   for (int i=0; i<24; ++i)
      rain += rainday[i];
   return rain/10;   
 }   
       
 
/*********************************************************************
**** Raw Measurements ****
*************************/   

float WeatherStation::rawWindSpeed ()
// Return the wind speed based on the number of wind ticks since the last poll.
// 1.492 mph is equivalent to 1 click per second.
 { float deltaTime = (millis() - lastpoll) / 1000.0;
   float windSpeed = 1.492 * windticks / deltaTime;
   return windSpeed;
 }

float WeatherStation::rawWindDirection()
 { // Try this 3 times incase the reading switches during sampling and we get a dud value.
   for (int i = 0; i<3; ++i)
    { unsigned int adc = analogRead(wdout); // get the current reading from the sensor
   
      // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
      // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
      // Note that these are not in compass degree order! See Weather Meters datasheet for more information.
      if (adc < 380) return (113);
      if (adc < 393) return (68);
      if (adc < 414) return (90);
      if (adc < 456) return (158);
      if (adc < 508) return (135);
      if (adc < 551) return (203);
      if (adc < 615) return (180);
      if (adc < 680) return (23);
      if (adc < 746) return (45);
      if (adc < 801) return (248);
      if (adc < 833) return (225);
      if (adc < 878) return (338);
      if (adc < 913) return (0);
      if (adc < 940) return (293);
      if (adc < 967) return (315);
      if (adc < 990) return (270);
    }
   return (-1); // error, disconnected?
 }
   
      
/*********************************************************************
**** Interrupt Handlers ****
***************************/   
   
volatile unsigned long WeatherStation::rainlastIRQ;
volatile unsigned long WeatherStation::rainticks;
   
void WeatherStation::rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
 {  unsigned long now = millis(); // grab current time
    if (now - rainlastIRQ > 10) // ignore switch-bounce glitches less than 10mS after initial edge
     { rainlastIRQ = now;
       rainticks++;      
     }
 }

volatile unsigned long WeatherStation::windlastIRQ;
volatile unsigned long WeatherStation::windticks;

void WeatherStation::wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
 { unsigned long now = millis();
   if (now - windlastIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
    { windlastIRQ = now; //Grab the current time
      windticks++; //There is 1.492MPH for each click per second.
    }
 }
   


