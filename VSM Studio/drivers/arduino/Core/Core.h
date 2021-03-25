/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****               Core Utility Classes and Types             *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#ifndef __CORE_H__
#define __CORE_H__


#include <arduino.h>
#include <print.h>
#include <stream.h>
#include <stdlib.h>
#include <math.h>
#include <timelib.h>

// VSM Specfic
#include "vsm.h"

// Prevent F("") macro from producing compiler warnings:
// See: http://forum.arduino.cc/index.php/topic,102182.0.html
#undef  PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))

#ifndef analogInputToDigitalPin
#define analogInputToDigitalPin analogPinToChannel
#endif


extern const char PROGMEM H_RESOURCE[]; 
extern const char PROGMEM H_FILE[]; 

typedef time_t Time;

struct Handle
 { PGM_P type;
    union 
     { const char *resource;
       class File *file;
       void *ptr;
     };
 };

class PrintBuffer : public Print, public Printable
 { public:
      PrintBuffer();
      void clear() { s = (char *)NULL; }
      size_t length() { return s.length(); }
      String &buffer() { return s; }
      virtual size_t write(uint8_t c);
      virtual size_t write(const uint8_t *buffer, size_t size);
      virtual size_t printTo(Print &buffer) const;      
      
   private:
      String s;
 };      

class PrintList 
 { public:
      PrintList ();
      void setDelimiter(char d) { delimiter = d; }
      void setTerminator(char t) { terminator = t; }
      void setPlaces (int p) { places = p; }
      void setBase(int b) { base = b; }
      virtual PrintList &begin(bool eol, Print *p);
      PrintList &arg(int i);
      PrintList &arg(long i);
      PrintList &arg(double f);
      PrintList &arg(const char *s);
      PrintList &arg(const String &s);
      PrintList &arg(const __FlashStringHelper *s);
      PrintList &arg(Time t);
      virtual void end();      
    
   protected:      
      void nextItem();
      void toEscaped(const String &s);
      
   protected:
      Print *target;
      bool newLine;
      uint8_t argCount;
      char delimiter;
      char terminator;
      int places;
      int base;      
 };

class StreamList 
 { public:
      StreamList ();
      void setDelimiter(char d) { delimiter = d; }
      void setTerminator(char t) { terminator = t; }

      virtual StreamList &begin(Stream *s);
      StreamList &arg(int &i); 
      StreamList &arg(long &i); 
      StreamList &arg(float &f);
      StreamList &arg(String &s);
      StreamList &arg(Time &t);
      virtual void end() { }
      
   private:      
      void skipDelimiter();
      bool isSpace(char c);
      String fromEscaped(char quote);
      
   private:
      Stream *source;
      char delimiter;
      char terminator;
 };


// Conversion functions:
inline long toInt(const String &s) { return atoi(s.c_str()); }
inline float toFloat(const String &s) { return atof(s.c_str()); }
extern String toString(long l);
extern String toString(float f);

// String library:
inline unsigned len(const String &s) { return s.length(); }
inline String left(const String &s, unsigned count) { return s.substring(0, count); }
inline String right(const String &s, unsigned count) { return s.substring(s.length()-count, s.length()-1); }
inline String sub(const String &s, unsigned from, unsigned to) { return s.substring(from, to); }
inline String upper(String s)    { s.toUpperCase(); return s; }
inline String lower(String s) { s.toLowerCase(); return s; }

// Time library
inline uint8_t tmHour(Time t) { return hour(t); }
inline uint8_t tmMinute(Time t) { return minute(t); }
inline uint8_t tmSecond(Time t) { return second(t); }
inline uint16_t tmYear(Time t) { return year(t); }
inline uint8_t tmMonth(Time t) { return month(t); }
inline uint8_t tmDay(Time t) { return day(t); }
inline uint8_t tmWeekDay(Time t) { return dayOfWeek(t); }
inline String monthName(uint8_t t) { return monthStr(t); }
inline String shortMonthName(uint8_t t) { return monthShortStr(t); }
inline String dayName(uint8_t t) { return dayStr(t); }
inline String shortDayName(uint8_t t) { return dayShortStr(t); }
inline bool   timeValid() { return timeStatus() == timeSet; }

Time makeTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
String time(Time t); 
String date(Time t); 


#endif
