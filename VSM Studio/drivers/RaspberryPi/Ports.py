import smbus
import spidev
from wiringpi import Serial
import time
from PrintList import PrintList

class I2c:
   def __init__(self):
      self.bus = smbus.SMBus(1)
      #port 0 = pins 27&28
      #port 1 = pins 3&5 
      
   def transmit(self,address,command,*values):
      array = []
      for i in range(len(values)):
         array.append(values[i])
      self.bus.write_i2c_block_data(address, command, array)
      
   def receive(self,address,command,output=[]):
      value = self.bus.read_i2c_block_data(address,command)
      for i in range(min(len(value),len(output))):
         output[i] = value[i]
         
    
class SPI:
   def setup(self,clock,device):
      self.spi = spidev.SpiDev()
      self.spi.open(0,device)
      self.spi.max_speed_hz = clock
      
   def transfer(self, output=[]):
      data = self.spi.xfer2(output)
      for i in range(len(output)):
         output[i] = data[i]

   def send(self,*values):
      list = []
      for i in range(len(values)):
         list.append(values[i])
      self.spi.xfer2(list)
      
   def receive(self,length):
      list = []
      for i in range(length):
         list.append(0xFF)
      data = self.spi.xfer2(list)
      return(data)
      
class UART(PrintList):
   def __init__(self):
      PrintList.__init__(self, self._print)
      self.uart = Serial("/dev/serial0", 9600)

   def setup(self, speed):
      self.uart = Serial("/dev/serial0", speed)
      
   def _print(self,string):
      self.uart.puts(string)
      
   def println(self,string):
      self.uart.puts(string + "\r")
      
   def recv(self):
      return self.uart.getchar()
   
   def send(self,char):
      self.uart.putchar(char)
      
   def input(self, echo=True):
      data = ""
      while True:
         input = self.uart.getchar()
         if echo:
            self.uart.putchar(input)
         if input == "\r":
            return(data)
         data += input

class SerialTerminal(UART):

   def __init__(self,Speed):
      PrintList.__init__(self, self._print)
      self.uart = Serial("/dev/serial0", Speed)