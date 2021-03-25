/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                    Proteus IoT Builder                   *****/
/*****                                                          *****/
/*****                    IoT Control Classes                   *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#ifndef VFPSERVER
#error VFP server object not found
#endif

#include <vsm.h>

class Indicator : public Control
 { public:
      Indicator (const char *_id) : Control(_id) {}
      void setState(int n) { VFP.set(id, F("state")).arg(n).end(); }
      void setState(long n) { VFP.set(id, F("state")).arg(n).end(); }
      void setState(float n) { VFP.set(id, F("state")).arg(n).end(); }
      void setState(const String &s) { VFP.set(id, F("state")).arg(s).end(); }
      void setState(const char *s) { VFP.set(id, F("state")).arg(s).end(); }
      void setValue(float n) { VFP.set(id, F("value")).arg(n).end(); }
      void setLabel(const String &t) { VFP.set(id, F("label")).arg(t).end(); }
 };

typedef Indicator BarGraph;
typedef Indicator LED;
typedef Indicator PanelMeter;
typedef Indicator Speedometer;
typedef Indicator LoadingBar;
typedef Indicator Gauge;
typedef Indicator LEDStrip;
typedef Indicator Bearing;


class TrafficLights : public Control
 { public:
       TrafficLights (const char *_id) : Control(_id), state(0) {}
       void setState(uint8_t s) { VFP.set(id, F("state")).arg(state=s).end();   }
       void setRed(bool b) { setState(b ? (state | 1) : (state & ~1)); }
       void setAmber(bool b) { setState(b ? (state | 2) : (state & ~2)); }
       void setGreen(bool b) { setState(b ? (state | 4) : (state & ~4)); }
   private:
       uint8_t state;
 };


class Thermometer : public Control
 { public:
      Thermometer (const char *_id) : Control (_id) {}
      void setTemperature(float t) { VFP.set(id, F("temperature")).arg(t).end(); }
 };


template<class T>
class Actuator : public Control
 { public:
      Actuator (const char *_id) : Control(_id), state(0) { }
      StreamList &parseArgs(StreamList &request) {  return request.arg(state);  }
      const T &operator()() const { return state; }
      void begin(T s) { setState(s); }
      void setState(const T &s) { VFP.set(id, F("state")).arg(state=s).end(); }
      const T &getState() const { return state; }
      void setValue(T n) { VFP.set(id, F("value")).arg(state=n).end(); }
      const T &getValue() const { return state; }
   protected:
      T state;
 };

typedef Actuator<float> RotaryKnob;
typedef Actuator<float> Slider;
typedef Actuator<float> Dial;
typedef Actuator<int> RotarySwitch;
typedef Actuator<int> SlideSwitch;
typedef Actuator<int> SteeringWheel;
typedef Actuator<int> Throttle;
typedef Actuator<int> ToggleSwitch;

class Button : public Actuator<int>
 { public:
      Button (const char *_id) : Actuator<int> (_id)  {}
      void setLamp(bool f) { VFP.set(id, F("lamp")).arg(f).end(); }
 };

typedef Button PushButton;
typedef Button ButtonGroup;

class NumericDisplay : public Control
 { public:
      NumericDisplay (const char *_id, uint8_t size) : Control(_id) {}
      void clear() { VFP.call(id, F("clear")).end(); }
      void setValue(float value) { VFP.call(id, F("setValue")).arg(value).end(); }
      void setValue(long value) { VFP.call(id, F("setValue")).arg(value).end(); }
      void setValue(int value) { VFP.call(id, F("setValue")).arg(value).end(); }
      void setRange(long value) { VFP.call(id, F("setRange")).arg(value).end(); }
      void setDigit(uint8_t digit, uint8_t value) { VFP.call(id, F("setDigit")).arg(digit).arg(value).end(); }
      void setPoint(uint8_t digit, bool value) { VFP.call(id, F("setPoint")).arg(digit).arg(value).end(); }
      void setSegs(uint8_t digit, uint8_t value) { VFP.call(id, F("setSegs")).arg(digit).arg(value).end(); }
 };

class SegmentedDisplay : public Control, public PrintList
 { public:
      SegmentedDisplay (const char *_id, uint8_t size) : Control(_id) {}
      void clear() { VFP.set(id, F("value"), ""); }
      void setValue(double n) { begin(false, &value).arg(n).end(); }
      void setValue(long n) { begin(false, &value).arg(n).end(); }
      void setValue(int n) { return begin(false, &value).arg(n).end(); }
      void setError() { VFP.set(id, F("value"), F("ERR")); }
      PrintList &print() { return begin(false, &value); }
   private:
      void end() { VFP.set(id, F("value"), value.buffer()); value.clear(); }
   private:
      PrintBuffer value;
 };

class LineChart : public Control, PrintList
 { public:
      LineChart (const char *id);
      void setXrange (float min, float max) { VFP.set(id, F("XRange")).arg(min).arg(max).end(); }
      void setTimeRange (uint8_t unit, long range, bool absolute) { VFP.set(id, F("TimeRange")).arg(unit).arg(range).arg(absolute).end(); }
      void setYrange (uint8_t axis, float min, float max);
      void setDataFile (const String &fileName) ;
      void setPlaces(uint8_t n) { PrintList::setPlaces(n); }
      void showColumn (uint8_t column, bool visible);
      PrintList &writeData ();
      void clearData();
   protected:
      void end();
   protected:
      String fileName;
      bool recording;
      PrintBuffer data;
 };

class Histogram : public Control, PrintList
 { public:
      Histogram (const char *id);
      void setXRange (float min, float max, int bars) { VFP.set(id, F("XRange")).arg(min).arg(max).arg(bars).end(); }
      void setTimeRange (uint8_t unit, long range, int bars, bool absolute) { VFP.set(id, F("TimeRange")).arg(unit).arg(range).arg(bars).arg(absolute).end(); }
      void setYRange (float min, float max);
      void setDataFile (const String &fileName);
      void setPlaces(uint8_t n) { PrintList::setPlaces(n); }
      void showColumn (uint8_t column, bool visible);
      PrintList &writeData ();
      void clearData();
   protected:
      void end();

   protected:
      String fileName;
      bool recording;
      PrintBuffer data;
 };

class BarChart : public Control, PrintList
 { public:
      BarChart (const char *id);
      ~BarChart();
      void init (int _catCount);
      void reload();

      void setSeriesVisible (uint8_t seriesIdx, bool visible);
      void setYRange (float min, float max);
      void setData (uint8_t catIdx, float value);
      void incrementData (uint8_t catIdx, float amount = 1.0f);
      void decrementData (uint8_t catIdx, float amount = 1.0f) { incrementData (catIdx, -amount); }
      void resetSeries(uint8_t seriesIdx);
      void writeSeries(String filename);
      void loadSeries(uint8_t seriesIdx, String filename);
      void clearData();

   protected:
      float * activeSeries;
      int8_t catCount;
 };

#define WIND_DIRS  16
#define WIND_BANDS 6

class WindRose : public Control
 { public:
      WindRose (const char *id);
      void begin (const String &file,  const String &bands);
      void setTimeRange (uint8_t unit, long range);
      void storeReading (float wind_dir, float wind_speed);
      void storeRecord ();
      void clearRecord();
      void clearData();

   private:
      String fileName;
      int thresholds[WIND_BANDS-1];
      uint8_t calm;
      uint8_t bins[WIND_DIRS][WIND_BANDS-1];
 };


class LineEdit : public Control
 { public:
      LineEdit (const char *_id) : Control(_id) {  }
      StreamList &parseArgs(StreamList &request) { return request.arg(text); }
      void setText(const String &t) { VFP.set(id, F("text")).arg(text=t).end(); }
      void setError(const String &t) { VFP.call(id, F("setError")).arg(t).end(); }
      const String &getText() const { return text; }
      void setReadOnly(bool readOnly) { VFP.set(id, F("readOnly")).end(); }
      void clear() { setText(F("")); }

   protected:
      String text;
 };

class Terminal : public Control, public PrintList
 { public:
      Terminal (const char *_id);
      StreamList &parseArgs(StreamList &request) { return request.arg(command); }
      void setPrompt(const String &t) { VFP.set(id, F("prompt")).arg(t).end(); }
      const String &getCommand () { return command; }
      void cls();
      PrintList &print();
      PrintList &println();

   protected:
      void end();

   protected:
      PrintBuffer text;
      String command;
 };

class TextLog : public Control, public PrintList
 { public:
      TextLog (const char *_id);
      void setFile (const String &fileName) ;
      PrintList &info();
      PrintList &warning();
      PrintList &error();
      void clear();

   protected:
      void end();

   private:
      String fileName;
      bool recording;
      PrintBuffer text;
 };

class AlertBox : public Control
 { public:
      AlertBox (const char *id) : Control (id) { }
      void reload() { VFP.create(id, F("AlertBox")); } // Create the JS object on a page reload
      void info (const String &msg) { VFP.call(id, F("info")).arg(msg).end();  }
      void warning (const String &msg) { VFP.call(id, F("warning")).arg(msg).end();  }
      void error (const String &msg) { VFP.call(id, F("error")).arg(msg).end();  }
      void confirm (const String &msg) { VFP.call(id, F("confirm")).arg(msg).end();  }
 };

class TextIn : public Control
 { public:
      TextIn (const char *_id) : Control(_id) {  }
      StreamList &parseArgs(StreamList &request) { return request.arg(text); }
      const String &operator()() const { return text; }
      const String &getInput () const { return text; }

   protected:
      String text;
 };

typedef TextIn KeyPad;

class Timer : public Control
 { public:
       Timer (const char *id);
       void begin (int8_t dir, uint8_t mode, uint8_t h, uint8_t m, uint8_t s, bool e);
       void poll();
       void reload() { sync(); }
       bool operator() () { bool t = triggered; triggered = false; return t; }
       void restart() { reset(true); }
       void reset (bool run=false);
       void start();
       void stop() { running = false; sync(); }
       void setPeriod(uint8_t hour, uint8_t min, uint8_t sec);
       void enableAlarm(bool flag) { enabled = flag; }
       const int getTime() { return time; }
   private:
      void trigger();
      void sync ();
   private:
       int8_t dir;
       uint8_t mode;
       time_t period, time;
       bool running, triggered;
       bool enabled;
       unsigned long nexttick;
 };

#define CLOCK_ALARMS 8

class Clock : public Control
 { public:
       Clock (const char *id);
       StreamList &parseArgs(StreamList &request);
       void poll();
       void reload() { sync(); }
       bool operator() (uint8_t alarm);
       void setTime(time_t time);
       time_t getTime() { return time; }
       void setTimeMode(uint8_t m);
       void cancelSetMode() { VFP.call(id, F("cancelMode")).end(); }
       void setAlarm(uint8_t id, uint8_t hour, uint8_t min);
       void enableAlarm(uint8_t id, bool flag) { enabled[id-1] = flag; }
       time_t getAlarm(uint8_t id) { return alarms[id-1]; }
       void attachAlarmHandler(void (*isr)()) { alarmHandler=isr; }
   private:
      void trigger();
      void sync ();
   private:
       time_t time, alarms[CLOCK_ALARMS];
       bool enabled[CLOCK_ALARMS];
       void (*alarmHandler)();
       unsigned long nexttick;
 };

 class JoyStick : public Control
 { public:
      JoyStick (const char *_id) : Control(_id), x(0), y(0){ }
      StreamList &parseArgs(StreamList &request) { return request.arg(x).arg(y);  }
      void begin(int x, int y) { setPos(x, y); }
      void setPos(int _x, int _y) { VFP.set(id, F("value")).arg(_x).arg(_y).end(); }
      const int &getValueX() const { return x; }
      const int &getValueY() const { return y; }
      void getValues(long *ptr_x, long *ptr_y) { *ptr_x = x; *ptr_y = y; }
   protected:
      int x, y;                                                                        
 };