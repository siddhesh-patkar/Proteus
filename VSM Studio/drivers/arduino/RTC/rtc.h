
#pragma once

#include <core.h>
#include <ds1307.h>
#include <timelib.h>

template<class T>
class RTC : public T
 { public:
      RTC() { instance=this;  timerInterruptHandler = alarmInterruptHandler = NULL; nextInterrupt=nextAlarm = 0; }
      
      void begin(uint8_t sout=0) { T::begin(); setSyncProvider(&RTC<T>::sync); }      
      void writeTime(void) { T::setTime(); }
      void readTime(void)  { T::getTime(); }        

      void setTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);            
      void setTimer(long interval) { nextInterrupt=now()+interval; timerInterval=interval; }
      void cancelTimer() { nextInterrupt = 0; }
      void setAlarm(uint8_t h, uint8_t m, uint8_t s) { nextAlarm=makeTime(year(), month(), day(), h, m, s); }
      void cancelAlarm() { nextAlarm = 0; }
      void attachTimerInterruptHandler(void (*isr)()) { timerInterruptHandler = isr; }
      void attachAlarmInterruptHandler(void (*isr)()) { alarmInterruptHandler = isr; }
      
      void poll(); 
       
   private:
      static RTC<T> *instance;
      static time_t sync();
      time_t nextInterrupt;
      time_t timerInterval;
      time_t nextAlarm;
      void (*timerInterruptHandler)();
      void (*alarmInterruptHandler)();
 };


template<class T> void RTC<T>::poll ()
 { time_t tnow = now();
   if (nextInterrupt && tnow >= nextInterrupt && timerInterruptHandler != NULL) 
    { (*timerInterruptHandler)(); 
      if (timerInterval) 
         nextInterrupt+=timerInterval; 
      else 
         nextInterrupt=0; 
    }
   if (nextAlarm && tnow >= nextAlarm && alarmInterruptHandler != NULL)
    { (*alarmInterruptHandler)();
      nextAlarm += SECS_PER_DAY;
    }
 }

template<class T> RTC<T> *RTC<T>::instance;
   
// DS1307 specific   

typedef RTC<DS1307> DS1307RTC;

template<> void RTC<DS1307>::setTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
 { setYear(year);
   setMonth(month);
   setDayOfMonth(day);
   setHour(hour);
   setMinute(minute);
   setSecond(second);
   writeTime();
   ::setTime(hour, minute, second, day, month, year);
 }

template<> time_t RTC<DS1307>::sync()
 { TimeElements te;
   instance->readTime();
   te.Hour = instance->hour;
   te.Minute = instance->minute;
   te.Second = instance->second;
   te.Year  = CalendarYrToTm(2000+instance->year);
   te.Month = instance->month;
   te.Day   = instance->dayOfMonth;
   return makeTime(te);
 }
 
