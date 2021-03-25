import grovepi
import grove_rgb_lcd
import time
import math
import grove_128_64_oled as oled
import smbus
from PrintList import PrintList
from wiringpi import Serial
#DONE

class Grove:
      def pinMode(self, pin, mode):
         grovepi.pinMode(pin, mode)

      def digitalWrite(self, pin, value):
         grovepi.digitalWrite(pin, value)

      def digitalRead(self, pin):
         grovepi.digitalRead(pin)

      def analogWrite(self, pin, value):
         grovepi.analogWrite(pin, value)

      def analogRead(self, pin):
         grovepi.analogRead(pin)


class GroveLED:
      __pin = 0

      def __init__(self, id):
         self.__pin = id
         grovepi.pinMode(self.__pin, "OUTPUT")

      def on(self):
         grovepi.digitalWrite(self.__pin, 1)

      def off(self):
         grovepi.digitalWrite(self.__pin, 0)

      def set(self, s):
         if (s):
            grovepi.digitalWrite(self.__pin, 1)
         else:
            grovepi.digitalWrite(self.__pin, 0)

      def dim(self, v):
         grovepi.analogWrite(self.__pin, v)

      def toggle(self):
         if (grovepi.digitalRead(self.__pin)):
            grovepi.digitalWrite(self.__pin, 0)
         else:
            grovepi.digitalWrite(self.__pin, 1)

      def __call__(self):
         return grovepi.digitalRead(self.__pin) == 1

class GroveButton:
      __pin = 0
      
      def __init__(self, id):
         self.__pin = id
         grovepi.pinMode(self.__pin, "INPUT")

      def __call__(self):
         return grovepi.digitalRead(self.__pin) == 1

class GroveSwitch:
      __pin = 0
      def __init__(self, id):
         self.__pin = id
         grovepi.pinMode(self.__pin, "INPUT")

      def __call__(self):
         return grovepi.digitalRead(self.__pin) == 1

class GroveRelay:
      __pin = 0
      def __init__(self, id):
         self.__pin = id
         grovepi.pinMode(self.__pin, "OUTPUT")
         
      def on(self):
         grovepi.digitalWrite(self.__pin, 1)

      def off(self):
         grovepi.digitalWrite(self.__pin, 0)
         
      def set(self, s):
         if (s):
            grovepi.digitalWrite(self.__pin, 1)
         else:
            grovepi.digitalWrite(self.__pin, 0)
            
      def __call__(self):
         return grovepi.digitalRead(self.__pin) == 1

class GroveBuzzer:
    __pin = 0

    def __init__(self, id):
        self.__pin = id
        grovepi.pinMode(self.__pin, "OUTPUT")

    def on(self):
        grovepi.digitalWrite(self.__pin, 1)

    def off(self):
        grovepi.digitalWrite(self.__pin, 0)

    def set(self, s):
        if (s):
            grovepi.digitalWrite(self.__pin, 1)
        else:
            grovepi.digitalWrite(self.__pin, 0)

    def __call__(self):
        return grovepi.digitalRead(self.__pin) == 1

class GroveTouchSensor:
    __pin = 0

    def __init__(self, id):
        self.__pin = id
        grovepi.pinMode(self.__pin, "INPUT")

    def __call__(self):
        return grovepi.digitalRead(self.__pin) == 1

class GroveLedBar:
    __pin = 0

    def __init__(self, id):
        self.__pin = id
        grovepi.ledBar_init(self.__pin, 1)

    def setOrientation(self, greenToRed):
        grovepi.ledBar_orientation(self.__pin, greenToRed)

    def setLevel(self, level):
        grovepi.ledBar_setLevel(self.__pin, level)

    def setLed(self, led, state):
        grovepi.ledBar_setLed(self.__pin, led, state)

    def toggleLed(self, led):
        grovepi.ledBar_toggleLed(self.__pin,led)

    def setLeds(self, bits):
        grovepi.ledBar_setBits(self.__pin,bits)

    def getLeds(self):
        grovepi.ledBar_getBits(self.__pin)

class Grove4DigitDisplay:
   __pin = 0

   def __init__(self, id):
      self.__pin = id
      grovepi.fourDigit_init(self.__pin)
      
   def init(self):
      grovepi.fourDigit_off(self.__pin)

   def setBrightness(self, level):
      grovepi.fourDigit_brightness(self.__pin, level)
        
   def display(self, pos, value):
      grovepi.fourDigit_number(self.__pin, value, pos)
      
   #TODO Unsure of desired result for decimal
   '''     
   def decPoint(self, state):
      value = 0
      if state == True:
         value = 128
         
      grovepi.fourDigit_segment(self.__pin, 0,value)
      grovepi.fourDigit_segment(self.__pin, 1,value)
      grovepi.fourDigit_segment(self.__pin, 2,value)
      grovepi.fourDigit_segment(self.__pin, 3,value)
   ''' 
        
class GroveTemperatureSensor:
    __pin = 0

    def __init__(self, id):
        self.__pin = id

    def resistance(self):
        a = grovepi.analogRead(self.__pin)
        return((1023-a)*10000.0/a)

    def readKelvin(self):
        return(1.0/(math.log(self.resistance()/10000)/3975+1/298.15))

    def readCelcius(self):
        return(self.readKelvin() - 273.15)

    def readFarenheit(self):
        return(9*self.readKelvin() / 5 - 459.67)

    def __call__(self, trigger):
        return self.readCelcius() >= trigger

class GroveLightSensor:
    __pin = 0
    Gamma = -0.858
    R1Lux = 127410.0
    Rref = 10000.0
    Vcc = 5.0

    def __init__(self, id):
        self.__pin = id

    def adcval(self):
        return grovepi.analogRead(self.__pin)

    def resistance(self):
        vout = (self.Vcc / 1024.0 * self.adcval())
        return (self.Rref * self.Vcc / vout) - self.Rref

    def readLight(self):
        return (self.resistance() / self.R1Lux) ** (1.0 / self.Gamma)

    def readResistance(self):
        return self.resistance()

    def readRaw(self):
        return self.adcval()


    def __call__(self, trigger):
        return self.readLight() >= trigger
        
class GroveRotaryAngleSensor:
      __pin = 0
      Full_angle = 0
      
      def adcval(self):
         return grovepi.analogRead(self.__pin)
      
      def __init__(self, id, angle):
         self.__pin = id
         self.Full_angle = angle
      
      def readAngle(self):
         return self.adcval() / 1024 * self.Full_angle
         
      def readRaw(self):
         return self.adcval()
      
      
      def __call__(self, trigger):
        return self.readAngle() >= trigger
        
class GroveVoltageDivider:
      __pin = 0
      an = 0
      idx = 0
      divisor = 0
      
      
      def __init__(self, id, Divisor):
         self.__pin = id
         if Divisor == True:
            self.divisor = 10
         else:
            self.divisor = 3
            
      def readValue(self, num_average):
         self.an=0
         self.idx=0
         while self.idx<num_average:
            self.an = self.an + grovepi.analogRead(self.__pin)
            self.idx += 1
         return self.an/num_average
         
      def readAnalog(self):
         return self.readValue(1)
            
      def readAverageAnalog(self, navr):
         return self.readValue(navr)
         
      def readVoltage(self):
         return self.divisor*self.readAverageAnalog(100)*5.0/1023.0

      def __call__(self, trigger):
        return self.readVoltage() >= trigger
         
class GroveLuminanceSensor:
      __pin = 0
      
      def __init__(self, id):
         self.__pin = id

      def readLuminance(self):
         vout = self.voltage()
         lux = -14.266*(vout**3)+76.189*(vout**2)+389.65*vout-0.8606
         
         if (lux < 0.0):
            lux = 0.0
         return lux
         
      def adcval(self):
         return grovepi.analogRead(self.__pin)
         
      def voltage(self):
         return 5.0/1024.0 * self.adcval()
      
      def readVoltage(self):
         return self.voltage()
         
         
      def __call__(self, trigger):
         return self.readLuminance() >= trigger
         
class GroveInfraredProximitySensor:
   __pin = 0
   
   def __init__(self, id):
      self.__pin = id + 1
   
   def readVoltage(self):
      return self.voltage()
      
   def readDistance(self):
      return (self.voltage()**-1.219)*26.714
   
   def voltage(self):
      an=0
      idx=0
      while idx<20:
         an = an + grovepi.analogRead(self.__pin)
         idx+=1
      return an/20 * 5.0/1024.0
   
   def __call__(self, trigger):
      return self.readDistance() >= trigger
      
class GroveDifferentialAmplifier:
   __pin = 0
   
   def __init__(self, id):
      self.__pin = id
   
   def readAnalog(self):
      return self.readValue(1)
      
   def readAverageAnalog(self, navr):
      return self.readValue(navr)
   
   def readVoltage(self):
      return self.readAverageAnalog(100)*5.0/1023.0
      
   def readValue(self, num_average):
      an=0
      idx=0
      while idx<num_average:
         an = an + grovepi.analogRead(self.__pin)
         idx += 1
      return an/num_average
   
   def __call__(self, trigger):
      return self.readVoltage() >= trigger
      
class GroveSoundSensor:
   __pin = 0
   
   def __init__(self, id):
      self.__pin = id
      
   def readLevel(self):
      return grovepi.analogRead(self.__pin)
      
   def __call__(self, trigger):
      return self.readLevel() >= trigger

#TODOs
      
class GroveUltrasonic:
   __pin = 0
   
   def __init__(self, id):
      self.__pin = id
      
   def readCentimeters(self):
      return grovepi.ultrasonicRead(self.__pin)/0.38
      
   def readInches(self):
      return grovepi.ultrasonicRead(self.__pin)
      
   def __call__(self, trigger):
      return self.readLevel() >= trigger
      
class GroveRgbLcd(PrintList):
   
   def __init__(self):
      self.memory = ""
      PrintList.__init__(self, self._print)
   
   def _print(self,string):
      self.memory = str(string)
      grove_rgb_lcd.setText(self.toString(string))
      
   def clear(self):
      self.memory = ""
      grove_rgb_lcd.setText("")
      
   def display(self):
      grove_rgb_lcd.setText(self.memory)
      
   def noDisplay(self):
      grove_rgb_lcd.setText("")
   
   def setColor(self,r,g,b):
      try:
         grove_rgb_lcd.setRGB(r,g,b)
      except:
         pass
         
class OLEDdisplay(PrintList):
   
   def __init__(self,address):
      self.line = 0
      
      oled.address = 0x3C + address - 1
      oled.init()
      
      oled.clearDisplay()          #clear the screen and set start position to top left corner
      oled.setNormalDisplay()      #Set display to normal mode (i.e non-inverse mode)
      oled.setPageMode() 
      
      PrintList.__init__(self, self._print)
      
   def writeCommand(self,command):
      oled.sendCommand(command)
      
   def writeData(self,data):
      oled.sendData(data)
      
   def clearDisplay(self):
      oled.clearDisplay()
      
   def invertDisplay(self,invert):
      if invert:
         oled.setInverseDisplay()
      else:
         oled.setNormalDisplay()
         
   def _print(self, string):
      oled.putString(self.toString(string))
   
   def println(self, string):
      oled.putString(self.toString(string)+'\n')
      self.line += 1
      oled.setTextXY(0,self.line)

class SerialTerminal(PrintList):
   def __init__(self,Speed):
      PrintList.__init__(self, self._print)
      self.uart = Serial("/dev/serial0", Speed)
      
   def _print(self,string):
      self.uart.puts(string)
   
   def println(self,string):
      self.uart.puts(string + "\r")
      
   def input(self, echo=True):
      data = ""
      while True:
         input = self.uart.getchar()
         if echo:
            self.uart.putchar(input)
         if input == "\r":
            return(data)
         data += input
           
class GroveI2CAdc:
   def __init__(self,jumperTwo,jumperOne):
      self.bus = smbus.SMBus(1)
      self.address = 0x50 + jumperOne * 4 + jumperTwo
      self.bus.write_byte_data(self.address, 0x02,0x20)
      
   def readAnalog(self):
      data=self.bus.read_i2c_block_data(self.address, 0x00, 2)#Reading Data
      raw_val=(data[0]&0x0f)<<8 | data[1]#Formatting Data
      
      return(raw_val)
      
   def readAverageAnalog(self,navr):
      average = 0
      for i in range(0,navr):#Adding Data together
         data=self.bus.read_i2c_block_data(self.address, 0x00, 2)#Reading Data
         raw_val=(data[0]&0x0f)<<8 | data[1]#Formatting Data
         average += raw_val
      average /= navr
      
      return(average)
      
   def readVoltage(self):
      return(self.readAnalog()/682)

   