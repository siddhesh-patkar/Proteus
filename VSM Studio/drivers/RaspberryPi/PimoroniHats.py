#Automation Hat
import automationhat
import piglow
import pianohat

from smbus import SMBus
import wiringpi
import RPi.GPIO as GPIO
from ADS1x15 import ADS1015 as ADS1x15

class PimoroniAutomationHat:
   #This will also need the library sn3218.py
   def setRelay(self,relay,value):
      automationhat.relay[relay].write(value)
      
   def readInput(self,input):
      value = automationhat.input[input].read()
      return(value)
      
   def readADC(self,input):
      value = automationhat.analog[input].read()
      return(value)
      
   def output(self,output,value):
      automationhat.output[output].write(value)
      
class PimoroniPiGlowHat:
   def __init__(self):
      piglow.auto_update = True
      
   #This will also need the library sn3218.py
   def setLeg(self,leg,value):
      value = value*255
      if value <= 255:
         piglow.leg(leg,int(value))
      else:
         print("Value too big")
      
   def setColour(self,colour,value):
      value = value*255
      if value <= 255:
         piglow.colour(colour,int(value))
      else:
         print("Value too big")

   def setRing(self,ring,value):
      if -1 <= ring < 6:
         value = value*255
         if value <= 255:
            piglow.ring(ring,int(value))
         else:
            print("Value too big")
      else:
         print("Rings are between 0 and 5")
      
   def setLED(self,led,value):
      value = value*255
      if value <= 255:
         piglow.led(led,int(value))
      else:
         print("Value too big")
      
   def allOff(self):
      piglow.off()
      
   def allOn(self,value):
      value = value *255
      print(value)
      if value <= 255:
         piglow.all(int(value))
      else:
         print("Value too big")
         
class PimoroniExplorerHat:
   def __init__(self):
      wiringpi.wiringPiSetupGpio()
      
      self.i2c = SMBus(1)
      multi = self.i2c.read_byte_data(40, 0x2A)
      self.i2c.write_byte_data(40, 0x2A, multi & ~0x80)
      
      #Setting Pins
      pinArray = [4,17,27,5,6,12,13,16]
      for i in range(len(pinArray)):
         wiringpi.pinMode(pinArray[i], wiringpi.OUTPUT)
         
      pinArray = [23,22,24,25]
      for i in range(len(pinArray)):
         wiringpi.pinMode(pinArray[i], wiringpi.INPUT)
         
      self.ads = ADS1x15(0x48)
      
      GPIO.setmode(GPIO.BCM)
      
      GPIO.setup(19, GPIO.OUT, initial=GPIO.LOW)
      GPIO.setup(20, GPIO.OUT, initial=GPIO.LOW)
      GPIO.setup(21, GPIO.OUT, initial=GPIO.LOW)
      GPIO.setup(26, GPIO.OUT, initial=GPIO.LOW)
      
      self.motor1b = GPIO.PWM(19, 100)
      self.motor1f = GPIO.PWM(20, 100)
      self.motor2b = GPIO.PWM(21, 100)
      self.motor2f = GPIO.PWM(26, 100)
      
      self.motor1b.start(0)
      self.motor1f.start(0)
      self.motor2b.start(0)
      self.motor2f.start(0)
      
   def setOutput(self,pin,value):
      pinArray = [6,12,13,16]
      if value:
         wiringpi.digitalWrite(pinArray[pin-1], wiringpi.HIGH)
      else:
         wiringpi.digitalWrite(pinArray[pin-1], wiringpi.LOW)
         
   def readAnalog(self,pin):
      value = (self.ads.read_adc(-(pin-4)))/(450)
      return value
      
   def readInput(self,pin):
      pinArray = [23,22,24,25]
      return wiringpi.digitalRead(pinArray[pin-1]) == wiringpi.HIGH
      
   def setMotor(self,motor,speed):
      pinArray = [19,20,21,26]

      if motor == 1 and speed >= 0:
         self.motor1f.ChangeDutyCycle(speed)
         self.motor1b.ChangeDutyCycle(0)
         
      elif motor == 1 and speed <= 0:
         self.motor1f.ChangeDutyCycle(0)
         self.motor1b.ChangeDutyCycle(-speed)
         
      elif motor == 2 and speed >= 0:
         self.motor2f.ChangeDutyCycle(speed)
         self.motor2b.ChangeDutyCycle(0)
         
      elif motor == 2 and speed <= 0:
         self.motor2f.ChangeDutyCycle(0)
         self.motor2b.ChangeDutyCycle(-speed)
         
      
   def setLight(self,led,value):
      pinArray = [4,17,27,5]
      if value:
         wiringpi.digitalWrite(pinArray[led-1], wiringpi.HIGH)
      else:
         wiringpi.digitalWrite(pinArray[led-1], wiringpi.LOW)
   
   def getButton(self,button):
      #order of buttons on pins
      buttonOrder = [3,2,1,0,7,6,5,4]
      
      reading = self.i2c.read_i2c_block_data(40, 0, 4)[3]
      array = [0,0,0,0,0,0,0,0]
      n = 128
      for i in range(0,8):
         if reading-n >= 0:
            reading -= n
            array[i] = 1
         n /=2
      
      self.i2c.write_byte_data(40, 0, 0)
      return array[buttonOrder[button-1]]

class PimoroniPianoHat:

   def buttonState(self,Button):
      state = pianohat.get_state()
      #print(state)
      return(state[Button])
      
   def setAutoLED(self,Boolean):
      pianohat.auto_leds(Boolean)
      
   def setLED(self,LED,State):
      pianohat.set_led(LED,State)