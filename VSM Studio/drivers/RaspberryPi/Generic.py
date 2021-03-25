import wiringpi
import time
import math
from Adafruit_MCP230xx.Adafruit_MCP230xx import Adafruit_MCP230XX
from ADS1x15 import ADS1015 as ADS1x15
import Adafruit_MCP3008
import math

class RelayBoards:
      __pinR1 = 0
      __pinR2 = 0
      __pinR3 = 0
      __pinR4 = 0

      def __init__(self, R1, R2, R3=2, R4=3):
         time.sleep(0.01)
         
         wiringpi.wiringPiSetupGpio()
         self.__pinR1 = R1
         wiringpi.pinMode(self.__pinR1, wiringpi.OUTPUT)
         self.__pinR2 = R2
         wiringpi.pinMode(self.__pinR2, wiringpi.OUTPUT)
         self.__pinR3 = R3
         wiringpi.pinMode(self.__pinR3, wiringpi.OUTPUT)
         self.__pinR4 = R4
         wiringpi.pinMode(self.__pinR4, wiringpi.OUTPUT)
         
   
      def relayOn(self, n):
         __pinHold = self.getPin(n)
         wiringpi.digitalWrite(__pinHold, wiringpi.HIGH)

      def relayOff(self, n):
         __pinHold = self.getPin(n)
         wiringpi.digitalWrite(__pinHold, wiringpi.LOW)
         
      def relaySet(self, n, s):
         __pinHold = self.getPin(n)
        
         if s:
            wiringpi.digitalWrite(__pinHold, wiringpi.HIGH)
         else:
            wiringpi.digitalWrite(__pinHold, wiringpi.LOW)

      def relayGet(self, n):
         __pinHold = self.getPin(n)
         return wiringpi.digitalRead(__pinHold)

         
      def getPin(self, n):
         __pinHold = 0
         
         if n == 0:
            __pinHold = self.__pinR1
         elif n == 1:
            __pinHold = self.__pinR2
         elif n == 2:
            __pinHold = self.__pinR3
         else:
            __pinHold = self.__pinR4
         
         return __pinHold

class Buzzer:
      __pin = 0

      def __init__(self, id):
         time.sleep(0.01)
         wiringpi.wiringPiSetupGpio()
         self.__pin = id
         wiringpi.pinMode(self.__pin, wiringpi.OUTPUT)
      
      def on(self):
         wiringpi.digitalWrite(self.__pin, wiringpi.HIGH)
         
      def off(self):
         wiringpi.digitalWrite(self.__pin, wiringpi.LOW)
    
      def set(self,s):
         if s:
            wiringpi.digitalWrite(self.__pin, wiringpi.HIGH)
         else:
            wiringpi.digitalWrite(self.__pin, wiringpi.LOW)

class LED:
      __pin = 0

      def __init__(self, id):
         time.sleep(0.01)
         wiringpi.wiringPiSetupGpio()
         self.__pin = id
         wiringpi.pinMode(self.__pin, wiringpi.OUTPUT)
      
      def on(self):
         wiringpi.digitalWrite(self.__pin, wiringpi.HIGH)
         
      def off(self):
         wiringpi.digitalWrite(self.__pin, wiringpi.LOW)
    
      def set(self,s):
         if s:
            wiringpi.digitalWrite(self.__pin, wiringpi.HIGH)
         else:
            wiringpi.digitalWrite(self.__pin, wiringpi.LOW)
            
      def toggle(self):
         if wiringpi.digitalRead(self.__pin):
            wiringpi.digitalWrite(self.__pin, wiringpi.LOW)
         else:
            wiringpi.digitalWrite(self.__pin, wiringpi.HIGH)
            
      def __call__(self):
         return wiringpi.digitalRead(self.__pin) == 1

class Button:
      __pin = 0

      def __init__(self, id):
         time.sleep(0.01)
         wiringpi.wiringPiSetupGpio()
         self.__pin = id
         wiringpi.pinMode(self.__pin, wiringpi.INPUT)
         
      def __call__(self):
         return wiringpi.digitalRead(self.__pin) == 1

class Piezo:
      __pin = 0
      isEnabled = False
      driveState = False
      
      
      def __init__(self, id):
         time.sleep(0.01)
         wiringpi.wiringPiSetupGpio()
         self.__pin = id
         wiringpi.pinMode(self.__pin, wiringpi.OUTPUT)
      
      def drive(self):
         if (self.driveState):
            self.driveState = False
         else:
            self.driveState = True
            
         if (self.isEnabled):
            if (self.driveState):
               wiringpi.digitalWrite(self.__pin, wiringpi.HIGH)
            else:
               wiringpi.digitalWrite(self.__pin, wiringpi.LOW)
      
      def enable(self):
         self.isEnabled = True
      
      def disable(self):
         self.isEnabled = False
         wiringpi.digitalWrite(self.__pin, wiringpi.LOW)
      
class Switch:
      __pin = 0

      def __init__(self, id):
         time.sleep(0.01)
         wiringpi.wiringPiSetupGpio()
         self.__pin = id
         wiringpi.pinMode(self.__pin, wiringpi.INPUT)
         
      def __call__(self):
         return wiringpi.digitalRead(self.__pin) == 1
 
class RgbLedCc:
   __pinR = 0
   __pinG = 0
   __pinB = 0
   
   def __init__(self, r, g, b):
            time.sleep(0.01)
            wiringpi.wiringPiSetupGpio()
            self.__pinR = r
            self.__pinG = g
            self.__pinB = b
            wiringpi.pinMode(self.__pinR, wiringpi.OUTPUT)
            wiringpi.pinMode(self.__pinG, wiringpi.OUTPUT)
            wiringpi.pinMode(self.__pinB, wiringpi.OUTPUT)
            
   def set(self, r, g, b):
      if r == True:
         wiringpi.digitalWrite(self.__pinR, wiringpi.HIGH)
      else:
         wiringpi.digitalWrite(self.__pinR, wiringpi.LOW)
      
      if g == True:
         wiringpi.digitalWrite(self.__pinG, wiringpi.HIGH)  
      else:
         wiringpi.digitalWrite(self.__pinG, wiringpi.LOW)
      
      if b == True:
         wiringpi.digitalWrite(self.__pinB, wiringpi.HIGH)
      else:
         wiringpi.digitalWrite(self.__pinB, wiringpi.LOW)
      
   def toggle(self):
      if wiringpi.digitalRead(self.__pinR) == False:
         wiringpi.digitalWrite(self.__pinR, wiringpi.HIGH)
      else:
         wiringpi.digitalWrite(self.__pinR, wiringpi.LOW)
      
      if wiringpi.digitalRead(self.__pinG) == False:
         wiringpi.digitalWrite(self.__pinG, wiringpi.HIGH)  
      else:
         wiringpi.digitalWrite(self.__pinG, wiringpi.LOW)
      
      if wiringpi.digitalRead(self.__pinB) == False:
         wiringpi.digitalWrite(self.__pinB, wiringpi.HIGH)
      else:
         wiringpi.digitalWrite(self.__pinB, wiringpi.LOW)

class RgbLedCa:
   __pinR = 0
   __pinG = 0
   __pinB = 0
   
   def __init__(self, r, g, b):
            time.sleep(0.01)
            wiringpi.wiringPiSetupGpio()
            self.__pinR = r
            self.__pinG = g
            self.__pinB = b
            wiringpi.pinMode(self.__pinR, wiringpi.OUTPUT)
            wiringpi.pinMode(self.__pinG, wiringpi.OUTPUT)
            wiringpi.pinMode(self.__pinB, wiringpi.OUTPUT)
            
   def set(self, r, g, b):
      if r == True:
         wiringpi.digitalWrite(self.__pinR, wiringpi.LOW)
      else:
         wiringpi.digitalWrite(self.__pinR, wiringpi.HIGH)
      
      if g == True:
         wiringpi.digitalWrite(self.__pinG, wiringpi.LOW)  
      else:
         wiringpi.digitalWrite(self.__pinG, wiringpi.HIGH)
      
      if b == True:
         wiringpi.digitalWrite(self.__pinB, wiringpi.LOW)
      else:
         wiringpi.digitalWrite(self.__pinB, wiringpi.HIGH)
      
   def toggle(self):
      if wiringpi.digitalRead(self.__pinR) == False:
         wiringpi.digitalWrite(self.__pinR, wiringpi.LOW)
      else:
         wiringpi.digitalWrite(self.__pinR, wiringpi.HIGH)
      
      if wiringpi.digitalRead(self.__pinG) == False:
         wiringpi.digitalWrite(self.__pinG, wiringpi.LOW)  
      else:
         wiringpi.digitalWrite(self.__pinG, wiringpi.HIGH)
      
      if wiringpi.digitalRead(self.__pinB) == False:
         wiringpi.digitalWrite(self.__pinB, wiringpi.LOW)
      else:
         wiringpi.digitalWrite(self.__pinB, wiringpi.HIGH)

class MCP23017:

   def __init__(self):
      try:
         self.setAddress(0x20)
      except:
         pass

   def setAddress(self,address):
      self.mcp = Adafruit_MCP230XX(busnum = 1, address = address, num_gpios = 16)
      
   def pinMode(self, pin, mode):
      self.mcp.config(pin, mode)
      
   def pullUpPin(self, pin):
      self.mcp.pullup(pin, 1)
      
   def digitalWrite(self,pin,value):
      self.mcp.output(pin, value)
      
   def digitalRead(self,pin):
      value = self.mcp.input(pin)
      if value > 0:
         return(1)
      else:
         return(0)
       
class ADS1015:
   def __init__(self):
      self.setAddress(0x48)
      
   def readAnalogue(self,port):
      value = (self.ads.read_adc(port))/(5*100)
      return value
   
   def setAddress(self,address):
      self.ads = ADS1x15(address)
      
class MCP3008:
   def __init__(self, CLK, DOUT, DIN, CS):
      self.mcp = Adafruit_MCP3008.MCP3008(clk=CLK, cs=CS, miso=DOUT, mosi=DIN)
   
   def analogRead(self,pin):
      value = self.mcp.read_adc(pin)/205
      return(value)
      
       
#TODO
class RotaryAngleSensor:
   __pin = 0
   full_angle = 0
   
   def __init__(self, id, angle):
      time.sleep(0.01)
      self.__pin = id
      full_angle = angle
      wiringpi.wiringPiSetupGpio()
      wiringpi.pinMode(self.__pin, wiringpi.INPUT)
      
   def readAngle(self):
      return adcval/1024*full_angle
    
   def readRaw(self):
      return adcval()
   
   def adcval(self):
      return wiringpi.analogRead(self.__pin)
    
   def __call__(self, trigger):
      if (readAngle() >= trigger):
         return readAngle()
 
class DCMotorV1:
      dir = 0
      pwm = 0
      brake = 0
      running = False
      
      def __init__(self, d, p, b):
         time.sleep(0.01)
         self.dir = d
         self.pwm = p
         self.brake = b
         self.running = False
         
      def __call__(self):
         return self.running
         
      def begin(self):
         wiringpi.pinMode(self.dir, wiringpi.OUTPUT)
         wiringpi.pinMode(self.pwm, wiringpi.OUTPUT)
         wiringpi.pinMode(self.brake, wiringpi.OUTPUT)
         
         wiringpi.analogWrite(self.pwm, 0)
         wiringpi.digitalWrite(self.brake, wiringpi.LOW)
         
      def run(self, direction, speed):
         if (direction == 1):
            wiringpi.digitalWrite(self.dir, wiringpi.HIGH)
         else:
            wiringpi.digitalWrite(self.dir, wiringpi.LOW)
      
         wiringpi.digitalWrite(self.brake, wiringpi.LOW)
         wiringpi.analogWrite(self.pwm, speed)

      def stop(self):
         wiringpi.digitalWrite(self.brake, wiringpi.HIGH)
         wiringpi.analogWrite(self.pwm, 255)
         
      def release(self):
         wiringpi.digitalWrite(self.brake, wiringpi.LOW)
         wiringpi.analogWrite(self.pwm, 0)
         
class GPS:
   storedData = []

   def __init__(self):
      self.uart = wiringpi.Serial("/dev/serial0", 9600)
      
   def read(self):
      data = ""
      c = 0
      while True:
         input = self.uart.getchar()
         if input == "\r":
            c = c + 1
         if c > 3:
            return(data)
         data += input
   
   def returnFix(self):
      data = self.read().split('\n')
      for i in range(len(data)):
         if '$GPGGA' in data[i]:
            if data[i].split(',')[2][:2] == '':
               return(False)
            else:
               return(True)
   
   def getPosition(self):
      data = self.read().split('\n')
      
      for i in range(len(data)):
         if '$GPGGA' in data[i]:
            lat = data[i].split(',')[2]
            lon = data[i].split(',')[4]
            try:
               latitude = float(lat[:2])+(float(lat[2:])/60)
               longitude = float(lon[:3])+(float(lon[3:])/60)
            except:
               latitude = 0
               longitude = 0
      return(latitude,longitude)
      
   def getAltitude(self):
      data = self.read().split('\n')
      
      for i in range(len(data)):
         if '$GPGGA' in data[i]:
            try:
               altitude = float(data[i].split(',')[9])
            except:
               altitude = 0
      return(altitude)
      
   def getSatellites(self):
      data = self.read().split('\n')
      
      for i in range(len(data)):
         if '$GPGGA' in data[i]:
            try:
               satellites = int(data[i].split(',')[7])
            except:
               satellites = 0
      return(satellites)

   def getDistance(self, lon1, lat1, lon2, lat2):

    # convert decimal degrees to radians 
    lon1 = math.radians(lon1)
    lat1 = math.radians(lat1)
    lon2 = math.radians(lon2)
    lat2 = math.radians(lat2)

    # haversine formula 
    dlon = lon2 - lon1 
    dlat = lat2 - lat1 
    a = math.sin(dlat/2)**2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon/2)**2
    c = 2 * math.asin(math.sqrt(a)) 
    r = 6371000 #6371 Radius of earth in kilometers. Use 3956 for miles
    return(c * r)
      
   def getBearing(self,lat1,lon1,lat2,lon2):
      lat1 = math.radians(lat1)
      lat2 = math.radians(lat2)

      diffLong = math.radians(lon2 - lon1)

      x = math.sin(diffLong) * math.cos(lat2)
      y = (math.cos(lat1) * math.sin(lat2) - (math.sin(lat1)
          * math.cos(lat2) * math.cos(diffLong)))

      initial_bearing = math.atan2(x, y)
      initial_bearing = math.degrees(initial_bearing)
      compass_bearing = (initial_bearing + 360) % 360
      return compass_bearing