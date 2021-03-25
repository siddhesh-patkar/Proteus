# Standard functions

import wiringpi
from datetime import datetime
import time as _time
import math

# General Functions

def toInt(val):
   return int(val)

def toFloat(val):
   return float(val)

def toString(val):
   return str(val)


# String Functions

def left(s, c):
   return s[:c]

def right(s, c):
   return s[len(s)-c:]

def upper(s):
   return s.upper()

def lower(s):
   return s.lower()


# Math Functions

def fabs(val):
   return math.fabs(val)

def fmin(a, b):
   return min(a, b)

def fmax(a, b):
   return max(a, b)

def ceil(val):
   return math.ceil(val)
   
def floor(val):
   return math.floor(val)

def sqrt(val):
   return math.sqrt(val)

def log(val):
   return math.log(val)
   
def log10(val):
   return math.log10(val)
   
def exp(val):
   return math.exp(val)

def pow(x, y):
   return math.pow(x, y)

def sin(x):
   return math.sin(x)

def cos(x):
   return math.cos(x)

def tan(x):
   return math.tan(x)
   
def asin(x):
   return math.asin(x)

def acos(x):
   return math.acos(x)

def atan(x):   
   return math.atan(x)

def degrees(x):   
   return math.degrees(x)

def radians(x):   
   return math.radians(x)

   
# Date Time Functions

def millis():
   return wiringpi.millis()
   
def now():
   return datetime.today()

def date(t):
   return t.strftime("%Y-%m-%d")
   
def time(t):
   return t.strftime("%H:%M:%S")

def dateTime(t):
   return t.strftime("%Y-%m-%d %H:%M:%S")
   
def makeTime(year, month, day, hour, minute, second):
   newtime = _time.mktime({year, month, day, hour, minute, second, 0, 1, -1})
   # TBD: Set current time

def tmHour(t):
   return t.hour

def tmMinute(t):
   return t.minute

def tmSecond(t):
   return t.second

def tmMonth(t):
   return t.month

def tmYear(t):
   return t.year

def tmDay(t):
   return t.day

def tmWeekDay(t):
   return t.weekday()

def monthName(month):
   return _time.strftime("%B", _time.strptime(str(month), "%m"))

def dayName(day):
   day+=1
   while day >= 7:
      day -= 7
   return _time.strftime("%A", _time.strptime(str(day), "%w"))

def shortMonthName(month):
   return _time.strftime("%b", _time.strptime(str(month), "%m"))

def shortDayName(day):
   day+=1
   while day >= 7:
      day -= 7
   return _time.strftime("%a", _time.strptime(str(day), "%w"))

def sleep(s):
   _time.sleep(s)