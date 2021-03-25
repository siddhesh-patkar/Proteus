/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                    Proteus IoT Builder                   *****/
/*****                                                          *****/
/*****                         Timer Control                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "vfpserver.h"
#include "controls.h"

Timer::Timer (const char *id) : Control(id)
 { dir = 0;
   mode = 0;
   period = time = 0;
   running = triggered = 0;
 }


void Timer::begin (int8_t d, uint8_t m, uint8_t hour, uint8_t min, uint8_t sec, bool e)
 { dir = d;
   mode = m;
   period = time = sec+60L*min+3600L*hour;
   enabled = e;
   reset();
   nexttick = millis();
 } 

void Timer::poll ()
 { while ((long)(millis()-nexttick) >= 0)
    { if (running)
       { if (dir > 0 && time != period || dir < 0 && time != 0)
          { if (dir > 0 && ++time == period || dir < 0 && --time == 0)
               trigger();
            else
               sync();   
          }
       }
      nexttick += 1000;    
    } 
 }    

void Timer::start ()
 { if (dir > 0 && time == period || dir < 0 && time == 0)         
      reset(true);
   else   
    { running = true; 
      sync(); 
    }
 }

void Timer::reset (bool run)
 { running = run; 
   time = dir > 0 ? 0 : period;  
   sync();
 }

  
void Timer::setPeriod (uint8_t hour, uint8_t min, uint8_t sec)
 { period = sec+60L*min+3600L*hour;
   sync();
 }

void Timer::trigger()
 { if (mode == 0) // One-Shot
    { running = false;
      sync();
    }
   else
      restart();
   if (enabled)
    { triggered = true;      
      if (eventHandler != NULL)    
         eventHandler();         
      VFP.call(id, F("trigger")).end();         
    }      
 }
 
void Timer::sync ()
 { VFP.call(id, F("sync")).arg(hour(time)).arg(minute(time)).arg(second(time)).arg(enabled).end();
 }