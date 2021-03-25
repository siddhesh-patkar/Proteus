/****************************************************************************/	
//	Function: Cpp file for DS1307
//	Hardware: Grove - RTC
//	Arduino IDE: Arduino-1.0
//	Author:	 FrankieChu		
//	Date: 	 Jan 19,2013
//	Version: v1.0
//	by www.seeedstudio.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
/****************************************************************************/

#include <Wire.h>
#include "DS1307.h"

uint8_t DS1307::decToBcd(uint8_t val)
{
	return ( (val/10*16) + (val%10) );
}

//Convert binary coded decimal to normal decimal numbers
uint8_t DS1307::bcdToDec(uint8_t val)
{
	return ( (val/16*10) + (val%16) );
}

void DS1307::begin()
{
	Wire.begin();
}
/*Function: The clock timing will start */
void DS1307::startClock(void)        // set the ClockHalt bit low to start the rtc
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write((uint8_t)0x00);                      // Register 0x00 holds the oscillator start/stop bit
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
  second = Wire.read() & 0x7f;       // save actual seconds and AND sec with bit 7 (sart/stop bit) = clock started
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write((uint8_t)0x00);
  Wire.write((uint8_t)second);                    // write seconds back and start the clock
  Wire.endTransmission();
}
/*Function: The clock timing will stop */
void DS1307::stopClock(void)         // set the ClockHalt bit high to stop the rtc
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write((uint8_t)0x00);                      // Register 0x00 holds the oscillator start/stop bit
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
  second = Wire.read() | 0x80;       // save actual seconds and OR sec with bit 7 (sart/stop bit) = clock stopped
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write((uint8_t)0x00);
  Wire.write((uint8_t)second);                    // write seconds back and stop the clock
  Wire.endTransmission();
}
/****************************************************************/
/*Function: Read time and date from RTC	*/
void DS1307::getTime()
{
    // Reset the register pointer
	Wire.beginTransmission(DS1307_I2C_ADDRESS);
	Wire.write((uint8_t)0x00);
	Wire.endTransmission();  
	Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
	// A few of these need masks because certain bits are control bits
	second	  = bcdToDec(Wire.read() & 0x7f);
	minute	  = bcdToDec(Wire.read());
	hour	     = bcdToDec(Wire.read() & 0x3f);// Need to change this if 12 hour am/pm
	dayOfWeek  = bcdToDec(Wire.read());
	dayOfMonth = bcdToDec(Wire.read());
	month      = bcdToDec(Wire.read());
	year	     = bcdToDec(Wire.read());
}
/*******************************************************************/
/*Frunction: Write the time that includes the date to the RTC chip */
void DS1307::setTime()
{
	Wire.beginTransmission(DS1307_I2C_ADDRESS);
	Wire.write((uint8_t)0x00);
	Wire.write(decToBcd(second));// 0 to bit 7 starts the clock
	Wire.write(decToBcd(minute));
	Wire.write(decToBcd(hour));  // If you want 12 hour am/pm you need to set bit 6 
	Wire.write(decToBcd(dayOfWeek));
	Wire.write(decToBcd(dayOfMonth));
	Wire.write(decToBcd(month));
	Wire.write(decToBcd(year));
	Wire.endTransmission();
}


/* Get functions */ 

// Get HMS
uint8_t DS1307::getHour(void)
 { // get hour
   return hour;
 }

uint8_t DS1307::getMinute(void)
 { // get minute
   return  minute;
 }

uint8_t DS1307::getSecond(void)
 { // get second
   return second;
 }

// Get YMD
uint16_t DS1307::getYear(void)
 { // get year
   return year;
 }

uint8_t DS1307::getMonth(void)
 { // get month
   return month;
 }

uint8_t DS1307::getDayOfMonth(void)
 { // get day of month
   return dayOfMonth;
 }

// Get DOW
uint8_t DS1307::getDayOfWeek(void)
 { // get day of week
   return dayOfWeek;
 }

/* Set functions */ 

// Set HMS
void DS1307::setHour(uint8_t _hour)
 { // assign hour
   hour = _hour;
 }

void DS1307::setMinute(uint8_t _minute)
 { // assign minute
   minute = _minute;
 }

void DS1307::setSecond(uint8_t _second)
 { // assign second
   second = _second;
 }

// Set YMD
void DS1307::setYear(uint16_t _year)
 { // assign year
   year = _year-2000;
 }

void DS1307::setMonth(uint8_t _month)
 { // assign month
   month = _month;
 }

void DS1307::setDayOfMonth(uint8_t _day)
 { // assign day of month
   dayOfMonth = _day;
 }

// Set DOW
void DS1307::setDayOfWeek(uint8_t _dow)
 { // assign day of week
   dayOfWeek = _dow;
 }
