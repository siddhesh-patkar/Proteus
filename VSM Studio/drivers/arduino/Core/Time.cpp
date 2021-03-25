/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****            Time Functionality (extra to TimeLib)         *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/


#include <arduino.h>
#include <ctype.h>

#include "core.h"

time_t makeTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)  
// Convert separate calendar values to a timecode.
 { tmElements_t tm;
  tm.Year = CalendarYrToTm(year);
  tm.Month = month;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;
  return makeTime(tm);
 }  

String date(Time t) 
// Return the date as yyyy::mm:dd
 { char buffer[16]; 
   sprintf(buffer, "%04d-%02d-%02d", year(t), month(t), day(t)); 
   return buffer; 
 }      

String time(Time t)
// Return the time as hh::mm:ss
 { char buffer[16]; 
   sprintf(buffer, "%02d:%02d:%02d", hour(t), minute(t), second(t)); 
   return buffer; 
 }

PrintList &PrintList::arg (Time t) 
// Print a timecode as yyyy-mm-dd hh:mm:ss
 { if (target) 
    { nextItem(); 
      target->print(date(t));
      target->print(' ');
      target->print(time(t));
    } 
   return *this; 
 }

StreamList &StreamList::arg (Time &t)
// Parse a date/time from a timecode
 { if (source && source->available()) 
    { TimeElements te;
      te.Year  = CalendarYrToTm(source->parseInt()); source->find('-');
      te.Month = source->parseInt(); source->find('-');
      te.Day   =  source->parseInt();
      te.Hour   = source->parseInt(); source->find(':');
      te.Minute =  source->parseInt(); source->find(':');
      te.Second =  source->parseInt(); 
      t = makeTime(te);
      skipDelimiter();  
    } 
   else
      t = 0;    
   return *this; 
 }

