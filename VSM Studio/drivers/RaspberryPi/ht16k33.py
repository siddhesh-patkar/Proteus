from PIL import Image
from PIL import ImageDraw
from Adafruit_LED_Backpack import Matrix8x8 as MX1
from Adafruit_LED_Backpack import Matrix8x16 as MX2
from Adafruit_LED_Backpack import AlphaNum4
from Adafruit_LED_Backpack import SevenSegment as Seven

class Matrix8x8:
   def __init__(self):
      self.display = MX1.Matrix8x8()
      self.display.begin()
      self.image = Image.new('1', (8, 8))
      self.draw = ImageDraw.Draw(self.image)
      
   def setSingle(self,x,y,boolean):
      self.display.set_pixel(x, y, boolean)
      self.display.write_display()
      
   def drawLine(self,x1,y1,x2,y2):
      self.draw.line((x1,y1,x2,y2), fill=255)
      self.display.set_image(self.image)
      self.display.write_display()
      
   def drawRectangle(self,x,y,length,height,fill):
      self.draw.rectangle((x,y,length,height), outline=255, fill=int(fill)*255)
      self.display.set_image(self.image)
      self.display.write_display()
      
   def clear(self):
      self.display.clear()
      self.display.write_display()
      
class Matrix8x16:
   def __init__(self):
      self.display = MX2.Matrix8x16()
      self.display.begin()
      self.image = Image.new('1', (8, 16))
      self.draw = ImageDraw.Draw(self.image)
      
   def setSingle(self,x,y,boolean):
      self.display.set_pixel(x, y, boolean)
      self.display.write_display()
      
   def drawLine(self,x1,y1,x2,y2):
      self.draw.line((x1,y1,x2,y2), fill=255)
      self.display.set_image(self.image)
      self.display.write_display()
      
   def drawRectangle(self,x,y,length,height,fill):
      self.draw.rectangle((x,y,length,height), outline=255, fill=int(fill)*255)
      self.display.set_image(self.image)
      self.display.write_display()
      
   def clear(self):
      self.display.clear()
      self.display.write_display()
   
class AlphaNumeric:
   def __init__(self):
      self.display = AlphaNum4.AlphaNum4()
      self.display.begin()
   
   def printString(self,string):
      self.display.print_str(string)
      self.display.write_display()
      
   def printFloat(self, float, place):
      self.display.print_float(float, place)
      self.display.write_display()
      
   def printInt(self, int):
      self.display.print_float(int, 0)
      self.display.write_display()
      
   def printHex(self, hex):
      self.display.print_hex(hex)
      self.display.write_display()
   
   def clear(self):
      self.display.clear()
      self.display.write_display()
      
class SevenSegment:
   def __init__(self):
      self.display = Seven.SevenSegment()
      self.display.begin()
   
   def printString(self,string):
      self.display.print_number_str(string)
      self.display.write_display()
      
   def printFloat(self, float, place):
      self.display.print_float(float, place)
      self.display.write_display()
   
   def printInt(self, int):
      self.display.print_float(int, 0)
      self.display.write_display()
      
   def printHex(self, hex):
      self.display.print_hex(hex)
      self.display.write_display()
      
   def colon(self, bool):
      self.display.set_colon(bool)
      self.display.write_display()
   
   def clear(self):
      self.display.clear()
      self.display.write_display()
   