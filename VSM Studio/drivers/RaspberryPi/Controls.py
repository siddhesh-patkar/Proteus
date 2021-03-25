#********************************************************************#
#********************************************************************#
#*****                                                          *****#
#*****        L A B C E N T E R    E L E C T R O N I C S        *****#
#*****                                                          *****#
#*****                    Proteus IoT Builder                   *****#
#*****                                                          *****#
#*****                    IoT Control Classes                   *****#
#*****                                                          *****#
#********************************************************************#
#********************************************************************#

#TODO Control. reload, reloadHandler, eventHandler. Won't crash but also won't work.
#TODO PrintList. setPlace. Unknown where this is set in the C++.

import VFP
import os
import datetime
from PrintList import PrintList
from FileStore import FileStore

# Control class
class Control:
   def  __init__(self, id):
      global controls
      self.id = id
      VFP.controls.append(self)
   
   def getId(self):
      return self.id
   
   def reload(self):
      pass

   def parseArgs(self, args):
      return args
   
   def reloadHandler(self):
      pass
      
   def eventHandler(self, state):
      pass
      
   def _set(self, key, value):
      VFP.set(self.id, key, value)

# TrafficLights
class TrafficLights(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._state = 0
      self._label = ""
   def setState(self, state):
      self._state = state
      self._set("state", state)
   def setRed(self, state):
      self._state = state
      self._set("state", 1)
   def setAmber(self, state):
      self._state = state
      self._set("state", 2)
   def setGreen(self, state):
      self._state = state
      self._set("state", 4)      

# Display Controls
class Indicator(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._state = 0
      self._label = ""
   def setState(self, state):
      self._state = state
      self._set("state", state)
   def setValue(self, value):
      self._state = value
      self._set("value", value)
   def setLabel(self, label):
      self._label = label
      self._set("label", "'" + label + "'")
   def setTemperature(self, temp):
      self._state = temp
      self._set("temperature", temp)

class BarGraph(Indicator):    pass
class LED(Indicator):         pass
class PanelMeter(Indicator):  pass
class Speedometer(Indicator): pass
class LoadingBar(Indicator):  pass
class Gauge(Indicator):       pass
class LEDStrip(Indicator):    pass
class Bearing(Indicator):     pass
class Thermometer(Indicator): pass 

class SegmentedDisplay(Control, PrintList):
   def __init__(self, id, elements):
      Control.__init__(self, id)
      PrintList.__init__(self, self.setValue)
      self._state = 0
      self.handler = None    
   def clear(self):
      self._set("value","")
      
   def setValue(self, value):
      self._state = str(value)
      self._set("value", '"'+str(value)+'"')
      
   def setError(self):
      self._set("value", '"ERR"')

# Feedback Controls
class Actuator(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._state = 0
      self.handler = None
   def begin(self, state):
      self.setState(state)
   def setState(self, state):
      self._state = state
      self._set("state", state)
   def getState(self):
      return int(self._state)
   def setValue(self, value):
      self._state = value
      self._set("value", value)
   def getValue(self):
      return int(self._state)
   def __call__(self):
      return self._state
   def eventHandler(self, state):
      val = state.split("=")
      if len(val) == 1:
         self.setState(val[0])
      elif val[0] == "state":
         self.setState(val[1])
      elif val[0] == "value":
         self.setValue(val[1])
      if self.handler:
         self.handler()
   def attachEventHandler(self, handler):
      self.handler = handler
   #Additional define. don't need an exra class for button. 
   def setLamp(self, f):
      self._set("lamp", f)

class RotaryKnob(Actuator):   pass
class Slider(Actuator):       pass
class Dial(Actuator):         pass
class RotarySwitch(Actuator): pass
class SlideSwitch(Actuator):  pass
class SteeringWheel(Actuator):pass
class Throttle(Actuator):     pass
class ToggleSwitch(Actuator): pass

class PushButton(Actuator):   pass
class ButtonGroup(Actuator):  pass

class JoyStick(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._x = 0
      self._y = 0
      self.handler = None
   
   def begin(self, x,y):
      self.setValue([int(self._x), int(self._y)])
      
   def setValue(self, values):
      self._x = int(values[0])
      self._y = int(values[1])
      self._set("value", [int(self._x), int(self._y)])

   def getValueX(self):
      return int(self._x)   
   def getValueY(self):
      return int(self._y)
   def getValues(self):
      return self._x,self._y
   
   def __call__(self):
      return [self._x,self._y]
   def eventHandler(self, state):
      val = state.split("=")
      if val[1] != "NaN,NaN":
         self._x = int(val[1].split(",")[0])
         self._y = int(val[1].split(",")[1])
      else:
         self._set("value", [int(self._x), -int(self._y)])
      if self.handler:
         self.handler()
   def attachEventHandler(self, handler):
      self.handler = handler

class KeyPad(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._state = 0
      self.handler = None
      
   def setState(self, state):
      self._state = state
      self._set("state", state)
      
   def getInput(self):
      return self._state

   def __call__(self):
      return self._state
   def eventHandler(self, state):
      val = state.split("=")
      self.setState(val[0][1:-1])
      if self.handler:
         self.handler()
   def attachEventHandler(self, handler):
      self.handler = handler

# Text Controls (WORKING: 13/04/18)
class AlertBox(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._state = 0
      self.handler = None
   def setState(self, state):
      self._state = state
      self._set("state", state)
      
   def reloadHandler(self):
      VFP.create(self.id, "AlertBox")
         
   def info(self, message):
      VFP.call(self.id, "info", '"'+message+'"')
      
   def warning(self, message):
      VFP.call(self.id, "warning", '"'+message+'"')
      
   def error(self, message):
      VFP.call(self.id, "error", '"'+message+'"')
      
   def confirm(self, message):
      VFP.call(self.id, "confirm", '"'+message+'"')
   
   def __call__(self):
      return self._state
   def eventHandler(self, state):
      val = state.split("=")
      if len(val) == 1:
         self.setState(val[0])
      elif val[0] == "state":
         self.setState(val[1])
      elif val[0] == "value":
         self.setValue(val[1])
      if self.handler:
         self.handler()
   def attachEventHandler(self, handler):
      self.handler = handler

class LineEdit(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._state = ""
      self.handler = None
      
   def setText(self, text):
      self._state = text
      self._set("text", '"'+text+'"')
      
   def setError(self, text):
      self._state = text
      VFP.call(self.id,"setError", '"'+text+'"')
          
   def clear(self):
      self._state = ""
      self._set("text", "")
      
   def getText(self):
      return self._state
      
   def setReadOnly(self, bool):
      self._set("readOnly", bool)
      
   def __call__(self):
      return self._state
   def eventHandler(self, state):
      val = state.split("=")
      if val[1][0] == '"':
         val[1] = val[1][1:-1]
      self.setText(val[1])
      if self.handler:
         self.handler()
   def attachEventHandler(self, handler):
      self.handler = handler

class Terminal(Control, PrintList):
   def __init__(self, id):
      Control.__init__(self, id)
      PrintList.__init__(self, self.appendText)
      self._state = ""
      self.handler = None
   
   def begin(self, state):
      self._state = state

   def setPrompt(self, text):
      self._state = text
      self._set("prompt", '"'+text+'"')
          
   def cls(self):
      VFP.call(self.id, "cls")
      VFP.erase(self.id)
      
   def appendText(self,text):
      VFP.record(self.id, True)
      VFP.call(self.id,"appendText", '"'+text+'"')
         
   def getCommand(self):
      return self._state
      
   def __call__(self):
      return self._state
   def eventHandler(self, state):
      val = state.split("=")
      if val[0][0] == '"':
         val[0] = val[0][1:-1]
      self.begin(val[0])
      if self.handler:
         self.handler()
   def attachEventHandler(self, handler):
      self.handler = handler

class TextLog(Control, PrintList):
   def __init__(self, id):
      Control.__init__(self, id)
      PrintList.__init__(self, self.info)
      self._fileName = ""
      self._recording = False
      self._text = ""
   
   def clear(self):
      VFP.call(self.id, "clear")
      VFP.erase(self.id)
      os.remove("../" + self._fileName)
   
   def info(self,text):
      self.append(text)
         
   def warning(self,text):
      output = 'w\\r'+self.toString(text)
      self.append(output)
      
   def error(self,text):
      output = 'e\\r'+self.toString(text)
      self.append(output)
      
   def setFile(self,fileName):
      if self._recording:
         VFP.erase(self.id)
         VFP.record(self.id,False)
         self._recording = False
         
      self._fileName = fileName
      self._set("file", '"'+fileName+'"')
   
   def append(self, output):
      #Writing to the file
      if self._fileName != "":
         file = open("../"+self._fileName, "a+")
         file.write(str(output).replace("\\r", "\r")+'\n');
         file.close()
         
      elif self._recording == False:
         VFP.record(self.id, True)
         self._recording = True

      VFP.call(self.id,"appendLine", '"'+self.toString(output)+'"')

# Chart Controls             
class LineChart(Control):# TODO setPlaces

   def __init__(self, id):
      Control.__init__(self, id)
      self._fileName = ""
      self._recording = False
      self._data = ""
   
   def setPlaces(self, n):
      pass
   
   def append(self):
      #Writing to the file
      if self._fileName != "":
         if self._data != "":
            store = FileStore()
            file = store.open(self._fileName, "a+")
            store.write(file,str(self._data).replace("\\r", "\r")+'\n');
            store.close(file)
         
      elif not self._recording:
         VFP.record(self.id, True)
         self._recording = True
      
      VFP.call(self.id,"appendData", '"'+self._data+'"')
      
   def showColumn(self, column, visible):
      print("ColumnVisible" + str(column) )
      VFP.set(self.id,"columnVisible" + str(column) , str(visible))
      
   def setYrange(self, axis, min, max):
      VFP.set(self.id, "YRange", str(axis)+ ',' + str(min) + ',' + str(max))
      
   def setXrange(self, min, max):
      VFP.set(self.id, "XRange", str(min) + ',' + str(max))
      
   def setTimeRange(self, unit, range, absolute):
      VFP.set(self.id, "TimeRange", str(unit) +','+ str(range) +','+ str(int(absolute)))

   def setDataFile(self, file):
      if self._recording:
         VFP.erase(self.id)
         VFP.record(self.id, False)
         self._recording = False
      VFP.set(self.id,'data', '"'+file+'"')
      self._fileName = file
      self.append()
      
   def clearData(self):
      VFP.call(self.id, "clearData")
      VFP.erase(self.id)
      store = FileStore()
      store.remove(self._fileName)
      
   def writeData(self,*d):
      self._data = str(d[0])
      for i in range(len(d[1:])):
         self._data += "," + str(d[i+1])
      self.append()

class Histogram(Control):# TODO setPlaces
   def __init__(self, id):
      Control.__init__(self, id)
      self._fileName = ""
      self._recording = False
      self._data = ""
   

   def append(self):
      #Writing to the file
      if self._fileName != "":
         if self._data != "":
            store = FileStore()
            file = store.open(self._fileName, "a+")
            store.write(file,str(self._data).replace("\\r", "\r")+'\n');
            store.close(file)
         
      elif not self._recording:
         VFP.record(self.id, True)
         self._recording = True
      
      VFP.call(self.id,"appendData", '"'+self._data+'"')
   
   def setPlaces(self,n):
      pass
   
   def clearData(self):
      VFP.call(self.id, "clearData")
      VFP.erase(self.id)
      store = FileStore()
      store.remove(self._fileName)
   
   def setXRange(self, max, min, bars):
      VFP.set(self.id,'XRange',str(max) + ',' + str(min) + ',' + str(bars))
      
   def setTimeRange(self, unit, range, bars, absolute):
      VFP.set(self.id,'TimeRange',str(unit) + ',' + str(range) + ',' + str(bars)+ ',' + str(int(absolute)))
      
   def setYRange(self, min, max):
      VFP.set(self.id, "YRange", str(min) + ',' + str(max))
      
   def setDataFile(self, file):
      if self._recording:
         VFP.erase(self.id)
         VFP.record(self.id, False)
         self._recording = False
      VFP.set(self.id,'data', '"'+file+'"')
      self._fileName = file
      self.append()
      
   def writeData(self,bar,y):
      self._data = str(bar) + "," + str(y)
      self.append()

class BarChart(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self.activeSeries = []
      self.catCount = -1
      VFP.record(self.id, True)
   
   def init(self, _catCount):
      self.catCount = _catCount
      for i in range(self.catCount):
         self.activeSeries.append(0.0)
        
   def reload(self):
      for i in range(self.catCount):
         VFP.call(self.id,"setData", '"'+str(0)+'","'+str(i)+'","'+str(self.activeSeries[i])+'"')
      
   def setSeriesVisible(self, seriesIdx, visible):
      VFP.set(self.id,'seriesVisible',str(seriesIdx) + ',' + str(visible))
      
   def setYrange(self, min, max):
      VFP.set(self.id,'YRange',str(min) + ',' + str(max))
      
   def setData(self, catIdx, value):
      if(catIdx < self.catCount):
         VFP.call(self.id,"setData", '"'+str(0)+'","'+str(catIdx)+'","'+str(value)+'"')
         self.activeSeries[catIdx] = value
   
   def incrementData(self, catIdx, amount):
      if(catIdx < self.catCount):
         self.setData(catIdx,self.activeSeries[catIdx] + amount)
      
   def decrementData(self,catIdx, amount):
      self.incrementData(catIdx, -amount)
      
   def resetSeries(self, seriesIdx):
      VFP.call(self.id,"clearSeriesData", '"'+str(seriesIdx)+'"')
      if (seriesIdx == 0):
         for i in range(self.catCount):
            self.activeSeries[i] = 0.0
      
   def writeSeries(self, filename):
      file = open("../"+filename, "w")
      for i in range(self.catCount):
         file.write(str(self.activeSeries[i])+'\n')
      file.close()
      
   def loadSeries(self, seriesIdx, filename):
      VFP.set(self.id,'seriesDataFile',str(seriesIdx) + ',"' + str(filename)+'"')
      
   def clearData(self):
      VFP.call(self.id, "clearData")
      VFP.erase(self.id)

class WindRose(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self._fileName = ""
      self.WIND_BANDS = 6
      self.WIND_DIRS = 16
      self.thresholds = []
      self.calm = 0
      
      self.clearRecord()     

   def begin(self, file, bands):
      self._fileName = file
      self.thresholds = bands.split(',')
   
   def reload(self):
      VFP.call(self.id, "loadData")
      
   def setTimeRange(self, unit, range):
      VFP.set(self.id,'TimeRange',str(unit) + ',' + str(range))
      
   def storeReading(self, wind_dir, wind_speed):
      #This will store a reading into the array
      dir = round((self.WIND_DIRS*wind_dir/360.0)%self.WIND_DIRS)
      band = 0
      while(band < self.WIND_BANDS-1 and wind_speed > int(self.thresholds[band])):
         band += 1

      if (band == 0):
         self.calm += 1
      else:
         self.bins[dir][band-1] += 1
      
   def storeRecord(self):
      #Tis writes to the data file
      dateTime = str(datetime.datetime.now()).split('.')[0]
      file = open('../'+self._fileName, 'a+')
      file.write(dateTime+','+str(self.calm))
      for i in range(self.WIND_DIRS):
         for n in range(self.WIND_BANDS-1):
            file.write(','+str(self.bins[i][n]))
      file.write('\n')
      file.close()
      
      self.clearRecord()
      VFP.call(self.id, "loadData")
      
   def clearRecord(self):
      #This sets and resets the array called bins
      self.bins = []
      for i in range(self.WIND_DIRS):
         self.bins.append([0,0,0,0,0])
      self.calm = 0
      
   def clearData(self):
      VFP.call(self.id, "clearData")
      os.remove('../'+self._fileName)

#Time Controls
class Clock(Control):
   def __init__(self, id):
      Control.__init__(self, id)

      rawTime = str(datetime.datetime.now()).split('.')[0]
      self.time = datetime.datetime.strptime(rawTime,'%Y-%m-%d %H:%M:%S').timestamp()
      
      self.CLOCK_ALARMS = 8 #Max number of alarms
      
      self.alarms = [] #Time
      self.enabled = []#Is alarm enabled or not?
      now = str(datetime.datetime.now()).split('.')[0] #Ticking Time
      self.nexttick = datetime.datetime.strptime(now,'%Y-%m-%d %H:%M:%S').timestamp()
      
      
      for i in range(self.CLOCK_ALARMS):
         self.alarms.append(0)
         self.enabled.append(0) 
   #  
   def sync(self):
      now = datetime.datetime.fromtimestamp(self.time)
      hour = str(now).split(' ')[1].split(':')[0]
      minute = str(now).split(':')[1]
      second = str(now).split(':')[2].split('.')[0]
      
      alarm = False
      for i in range(self.CLOCK_ALARMS):
         if self.enabled[i] == 1:
            alarm = True
      
      VFP.call(self.id,"sync", hour+','+minute+','+second+',"'+str(alarm)+'"')
   
   def poll(self):#TODO poll at end of loop
      millis = datetime.datetime.strptime(str(datetime.datetime.now()).split('.')[0],'%Y-%m-%d %H:%M:%S').timestamp()
      while (millis - self.nexttick) >= 0:
         self.time = (self.time + 1)#%86400
         self.sync()
         
         for i in range(self.CLOCK_ALARMS):
            if ((self.enabled[i] == True) & (self.time == self.alarms[i])):
               self.trigger()
         
         self.nexttick += 1
   # 
   def reload(self):
      self.sync()
   # 
   def operator(self, id):
      id -= 1
      return self.enabled[id] & self.alarms[id] >= self.time  & self.time < self.alarms[id]+60;
   #
   def setTime(self, time):
      self.time = time
      self.sync()
   #
   def getTime(self):
      return(self.time)
   #
   def setTimeMode(self, id):
      h = 0
      m = 0
      now = datetime.datetime.fromtimestamp(self.time)
      if id == 0:
         h = str(now).split(' ')[1].split(':')[0]
         m = str(now).split(':')[1]
      else:
         h = int(self.alarms[id-1]/3600)
         m = self.alarms[id-1]/60-60*h

         
         
      VFP.call(self.id,"setMode", str(id)+','+str(h)+','+str(m))
   #
   def cancelSetMode(self):
      VFP.call(self.id,"cancelMode")
   # 
   def setAlarm(self,id,hour,min):
      if (id >= 1 & id <= self.CLOCK_ALARMS):
         self.alarms[id-1] = self.time + min*60 + hour*3600;
         self.enabled[id-1] = True
   #
   def enableAlarm(self,id,flag):
      self.enabled[id-1] = flag
   #
   def getAlarm(self, id):
      return(self.alarms[id-1])
   #
   def attachAlarmHandler(self, handler):
      self.alarmHandler = handler
   #
   def trigger(self):
      VFP.call(self.id,"trigger")
      if self.alarmHandler != None:
         self.alarmHandler()

class Timer(Control):
   def __init__(self, id):
      Control.__init__(self, id)
      self.dir = 0
      self.mode = 0
      self.period = 0
      self.time = 0
      self.running = False #Time
      self.triggered = 0
      self.millis = 0
      
      self.nexttick = datetime.datetime.strptime(str(datetime.datetime.now()).split('.')[0],'%Y-%m-%d %H:%M:%S').timestamp()
      
   def begin(self, d, m, hour, min, sec, e):
      self.dir = d
      self.mode = m
      self.time = sec + 60*min*3600*hour
      self.period = self.time
      if e == 0:
         self.enabled = False
      else:
         self.enabled = True
      self.reset(False)
      
   def poll(self):
      millis = datetime.datetime.strptime(str(datetime.datetime.now()).split('.')[0],'%Y-%m-%d %H:%M:%S').timestamp()
      while (millis - self.nexttick) >= 0: 
         if self.running:
            if ((int(self.dir) > 0) and (self.time != self.period)) or ((int(self.dir) < 0) and (self.time != 0)):
               if ((int(self.dir) > 0) and (self.time+1 == self.period)) or ((int(self.dir) < 0) and (self.time-1 == 0)):
                  self.trigger()
               else:
                  if int(self.dir) > 0:
                     self.time +=1
                  else:
                     self.time -= 1
                  self.sync()
      
         self.nexttick += 1

      
   def reload(self):
      self.sync()
      
   def operator(self):
      t = self.triggered
      self.triggered = False
      return t
      
   def restart(self):
      self.reset(True)

   def reset(self, run = False):
      self.running = run
      if int(self.dir) > 0:
         self.time = 0
      else:
         self.time = self.period
      self.sync()
      
   def start(self):
      if (((int(self.dir) > 0) and (self.time == self.period)) or ((int(self.dir) < 0) and (self.time == 0))):
         self.reset(True)
      else:
         self.running = True
         self.sync()
      
   def stop(self):
      self.running = False
      self.sync()
      
   def setPeriod(self,hour,min,sec):
      self.period = sec+(60*min)+(3600*hour)
      self.sync()
   
   def enableAlarm(self, flag):
      if flag == 0:
         self.enabled = False
      else:
         self.enabled = True
      
   def trigger(self):
      if int(self.mode) == 0:
         self.running = False
         if int(self.dir) > 0:
            self.time += 1
         else:
            self.time -= 1
         self.sync()
      else:
         self.restart()
         
      if self.enabled:
         self.triggered = True
         if self.eventHandler != None:
            self.eventHandler()
         VFP.call(self.id,"trigger")        
      
   def sync(self):
      hour = str(int(self.time/3600))
      minute = str(int((self.time%3600)/60))
      second = str(int((self.time%3600)%60))
      
      VFP.call(self.id,"sync", hour+','+minute+','+second+',"'+str(self.enabled)+'"')
      
   def getTime(self):
      return self.time

      
   def attachEventHandler(self, handler):
      self.eventHandler = handler
      
# Photo / Video controls
class Photo(Control):
   def __init__(self, id):
      Control.__init__(self, id)
   def showImage(self, file):
      self._set("file", "'"+file+"'")
   def showVideo(self, file):
      self._set("file", "'"+file+"'")
   def clear(self):
      VFP.call(self.id,"clear")
