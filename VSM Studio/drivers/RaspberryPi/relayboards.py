import wiringpi
import time
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
         return readingpi.digitalRead(__pinHold)

         
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
