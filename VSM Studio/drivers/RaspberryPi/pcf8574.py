# PCF8574 Port Extender Driver

import wiringpi

def readPort(address) :
   i2c = wiringpi.I2C()
   file = i2c.setupInterface("/dev/i2c-1", address)
   return i2c.read(file)

def writePort(address, data) :
   i2c = wiringpi.I2C()
   file = i2c.setupInterface("/dev/i2c-1", address)
   i2c.write(file, data)
   return
