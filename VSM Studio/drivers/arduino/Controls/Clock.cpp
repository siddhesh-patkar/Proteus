/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                    Proteus IoT Builder                   *****/
/*****                                                          *****/
/*****                       Clock Control                      *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "vfpserver.h"
#include "controls.h"

Clock::Clock (const char *id) : Control (id) 
 { time = 0;
   for (uint8_t i=0; i<CLOCK_ALARMS; ++i)
    { alarms[i] = 0;
      enabled[i] = 0;
    }
   alarmHandler = NULL; 
   nexttick = millis();
 }

StreamList &Clock::parseArgs(StreamList &request)
// Handle setting events from the control.
 { int id, hour=-1, min=-1, second=0;
   request.arg(id);
   if (id != -1)
    { if (id == 0)
       { request.arg(hour).arg(min).arg(second);
         setTime(min*60L+hour*3600L+second);
       }
      else 
       { request.arg(hour).arg(min);
         if (hour != -1 && min != -1)
          { setAlarm(id, hour, min);        
            enableAlarm(id, true);   
          }
         else 
            enableAlarm(id, false);   
       }
    }
   return request;
 }
   
void Clock::poll()
 { while ((long)(millis()-nexttick) >= 0)
    { time = (time+1) % 86400L;
      sync();
      for (uint8_t i=0; i<CLOCK_ALARMS; ++i)
         if (enabled[i] && time == alarms[i])
          { trigger();
            break;
          }
      nexttick += 1000;
    }   
 }

bool Clock::operator() (uint8_t id)
// Return true if the specified alarm is active.
// The alarm is deemed to be active for a whole minute.
 { id -= 1;
   return enabled[id] && alarms[id] >= time  && time < alarms[id]+60;
 }

void Clock::setTimeMode (uint8_t id)
 { uint8_t h, m;
   if (id == 0)
      h = hour(time), m=minute(time);
   else if (id <= CLOCK_ALARMS)
      h = hour(alarms[id-1]), m = minute(alarms[id-1]);
   VFP.call(this->id, F("setMode")).arg(id).arg(h).arg(m).end(); 
 }

void Clock::setTime (time_t t)
// Set the time and update the display.  
 { time = t;
   sync();
 }

void Clock::setAlarm(uint8_t id, uint8_t hour, uint8_t min)
// Set an alarm time.
 { if (id >= 1 && id <= CLOCK_ALARMS)
    { alarms[id-1] = min*60L + hour*3600L;
      enableAlarm(id, true);
    }       
 }

void Clock::trigger()
 { VFP.call(id, F("trigger")).end();
   if (alarmHandler != NULL)    
      alarmHandler();         
 }
 
void Clock::sync ()
 { bool alarm = false;
   for (uint8_t i=0; i<CLOCK_ALARMS; ++i)
      if (enabled[i])
         alarm = true;
   VFP.call(id, F("sync")).arg(hour(time)).arg(minute(time)).arg(second(time)).arg(alarm).end();
 }
